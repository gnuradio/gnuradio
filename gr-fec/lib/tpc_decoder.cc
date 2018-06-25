/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/fec/tpc_decoder.h>
#include <gnuradio/fec/tpc_common.h>
#include <volk/volk.h>

#include <math.h>
#include <boost/assign/list_of.hpp>
#include <sstream>
#include <stdio.h>
#include <vector>

#include <algorithm>            // for std::reverse
#include <string.h>             // for memcpy

#include <gnuradio/fec/maxstar.h>

namespace gr {
 namespace fec {

generic_decoder::sptr
tpc_decoder::make(std::vector<int> row_polys, std::vector<int> col_polys, int krow, int kcol, int bval, int qval, int max_iter, int decoder_type)
{
    return generic_decoder::sptr(new tpc_decoder(row_polys, col_polys, krow, kcol, bval, qval, max_iter, decoder_type));
}

tpc_decoder::tpc_decoder (std::vector<int> row_polys, std::vector<int> col_polys, int krow, int kcol, int bval, int qval, int max_iter, int decoder_type)
    : generic_decoder("tpc_decoder"), d_rowpolys(row_polys), d_colpolys(col_polys), d_krow(krow), d_kcol(kcol),
      d_bval(bval), d_qval(qval), d_max_iter(max_iter), d_decoder_type(decoder_type)
{
    // first we operate on data chunks of get_input_size()
    // TODO: should we verify this and throw an error if it doesn't match?  YES
    // hwo do we do that?

    rowEncoder_K = ceil(log(d_rowpolys[0])/log(2));    // rowEncoder_K is the constraint length of the row encoder polynomial
    rowEncoder_n = d_rowpolys.size();
    rowEncoder_m = rowEncoder_K - 1;
    colEncoder_K = ceil(log(d_colpolys[0])/log(2));    // colEncoder_K is the constraint length of the col encoder polynomial
    colEncoder_n = d_colpolys.size();
    colEncoder_m = colEncoder_K - 1;

    // calculate the input and output sizes
    inputSize = ((d_krow+rowEncoder_m)*rowEncoder_n)*((d_kcol+colEncoder_m)*colEncoder_n) - d_bval;
    outputSize = (d_krow*d_kcol - (d_bval+d_qval));

    fp = NULL;
    //DEBUG_PRINT("inputSize=%d outputSize=%d\n", inputSize, outputSize);
    //fp = fopen("c_decoder_output.txt", "w");

    rowNumStates = 1 << (rowEncoder_m);       // 2^(row_mm)
    colNumStates = 1 << (colEncoder_m);       // 2^(col_mm)
    rowOutputs.resize(2, std::vector<int>(rowNumStates,0));
    rowNextStates.resize(2, std::vector<int>(rowNumStates,0));
    colOutputs.resize(2, std::vector<int>(colNumStates,0));
    colNextStates.resize(2, std::vector<int>(colNumStates,0));;

    // precalculate the state transition matrix for the row polynomial
    tpc_common::precomputeStateTransitionMatrix_RSCPoly(rowNumStates, d_rowpolys, rowEncoder_K, rowEncoder_n,
                                                        rowOutputs, rowNextStates);

    // precalculate the state transition matrix for the column polynomial
    tpc_common::precomputeStateTransitionMatrix_RSCPoly(colNumStates, d_colpolys, colEncoder_K, colEncoder_n,
                                                        colOutputs, colNextStates);

    codeword_M = d_kcol + colEncoder_K - 1;
    codeword_N = d_krow + rowEncoder_K - 1;

    // pre-allocate memory we use for encoding
    inputSizeWithPad = inputSize + d_bval;

    channel_llr.resize(codeword_M, std::vector<float>(codeword_N, 0));
    Z.resize(codeword_M, std::vector<float>(codeword_N, 0));
    extrinsic_info.resize(codeword_M*codeword_N, 0);

    input_u_rows.resize(d_krow, 0);
    input_u_cols.resize(d_kcol, 0);
    input_c_rows.resize(codeword_N, 0);
    input_c_cols.resize(codeword_M, 0);
    output_u_rows.resize(d_krow, 0);
    output_u_cols.resize(d_kcol, 0);
    output_c_rows.resize(codeword_N, 0);

    output_c_cols.resize(codeword_M*codeword_N, 0);
    output_c_col_idx = 0;

    // setup the max_star function based on decoder type
    switch(d_decoder_type) {
        case 0:
            max_star = &tpc_decoder::linear_log_map;
            break;
        case 1:
            max_star = &tpc_decoder::max_log_map;
            break;
        case 2:
            max_star = &tpc_decoder::constant_log_map;
            break;
        case 3:
            max_star = &tpc_decoder::log_map_lut_correction;
            break;
        case 4:
            max_star = &tpc_decoder::log_map_cfunction_correction;
            break;
        default:
            max_star = &tpc_decoder::linear_log_map;
            break;
    }

	// declare the reverse sweep trellis
	// the beta vector is logically laid out in memory as follows, assuming the
	// following values (for educational purposes)
	// defined: LL = 6, rowEncoder_K = 3, derived: mm_row=2, max_states_row=4, rowEncoder_n=1, num_symbols_row=2
	// state_idx
	//-------------------------------------------------------------------------
	//    0    B(0,0) <-- the calculated beta value at state=0, time=0
	//
	//    1    B(0,1)
	//
	//    2    B(0,2)
	//
	//    3    B(0,3)
	//-------------------------------------------------------------------------
	//k(time) =   0     1     2     3     4     5     6     7     8
    // setup row siso decoder
    mm_row = rowEncoder_K-1;                // encoder memory
    max_states_row = 1 << mm_row;       	// 2^mm_row
    num_symbols_row = 1 << rowEncoder_n;    // 2^rowEncoder_n

    beta_row.resize(input_u_rows.size()+rowEncoder_K, std::vector<float>(max_states_row, 0));
    // forward sweep trellis for current time instant only (t=k)
    alpha_prime_row.resize(max_states_row, 0);
    // forward sweep trellis for next time instant only    (t=k+1)
    alpha_row.resize(max_states_row, 0);
    // likelihood vector that input_c[idx] corresponds to a symbol in the set of [0 ... num_symbols_row-1]
    metric_c_row.resize(num_symbols_row, 0);
    // temporary vector to store the appropriate indexes of input_c that we want to test the likelihood of
    rec_array_row.resize(rowEncoder_n, 0);
    // log-likelihood ratios of the coded bit being a 1
    num_llr_c_row.resize(rowEncoder_n, 0);
    // log-likelihood ratios of the coded bit being a 0
    den_llr_c_row.resize(rowEncoder_n, 0);


    // setup column siso decoder
    mm_col = colEncoder_K-1;                // encoder memory
    max_states_col = 1 << mm_col;       	// 2^mm_col
    num_symbols_col = 1 << colEncoder_n;    // 2^colEncoder_n
    beta_col.resize(input_u_cols.size()+colEncoder_K, std::vector<float>(max_states_col, 0));
	// forward sweep trellis for current time instant only (t=k)
	alpha_prime_col.resize(max_states_col, 0);
	// forward sweep trellis for next time instant only    (t=k+1)
	alpha_col.resize(max_states_col, 0);
	// likelihood vector that input_c[idx] corresponds to a symbol in the set of [0 ... num_symbols_col-1]
	metric_c_col.resize(num_symbols_col, 0);
	// temporary vector to store the appropriate indexes of input_c that we want to test the likelihood of
	rec_array_col.resize(colEncoder_n, 0);
	// log-likelihood ratios of the coded bit being a 1
	num_llr_c_col.resize(colEncoder_n, 0);
	// log-likelihood ratios of the coded bit being a 0
	den_llr_c_col.resize(colEncoder_n, 0);

    mInit = (d_bval+d_qval)/d_krow;		// integer division
    nInit = (d_bval+d_qval) - mInit*d_krow;

    numInitLoadIter = d_bval/codeword_N;
	numInitRemaining = d_bval - codeword_N*numInitLoadIter;

}

int tpc_decoder::get_output_size() {
    return outputSize;
}

int tpc_decoder::get_input_size() {
    return inputSize;
}

// this code borrows heavily from CML library, please look @:
// http://code.google.com/p/iscml
// it has been refactored to work w/ gnuradio in C++ environment,
// as well as some comments being added to help understand
// exactly what is going on w/ the siso decoder
void tpc_decoder::siso_decode_row() {

    int LL, state, k, ii, symbol, mask;
    float app_in, delta1, delta2;
    LL = input_u_rows.size();        // code length

    // log-likelihood ratio of the uncoded bit being a 1
    float num_llr_u;
    // log-likelihood ratio of the uncoded bit being a 0
    float den_llr_u;

    // initialize beta_row trellis
    // this initialization is saying that the likelihood that the reverse sweep
    // starts at state=0 is 100%, b/c state 1, 2, 3's likelihood's are basically -inf
    // this implies that the forward trellis terminated at the 0 state
    //    for(state=1; state<max_states_row; state++) {
    //		beta_row[LL+rowEncoder_K-1][state] = -MAXLOG;
    //	}
    // filling w/ 0xCC yields a value close to -MAXLOG, and memset is faster than for loops
    memset(&beta_row[LL+rowEncoder_K-1][1], 0xCC, sizeof(float)*(max_states_row-1));

    // initialize alpha_prime_row (current time instant), alpha_prime_row then gets updated
    // by shifting in alpha_row at the end of each time instant of processing
    // alpha_row needs to get initialized at the beginning of each processing loop, so we
    // initialize alpha_row in the forward sweep processing loop
    // the initialization below is saying that the likelihood of starting at state=0 is
    // 100%, b/c state 1, 2, 3's likelihoods are basically -inf
    // as with the beta_row array, this implies that the forward trellis started at state=0
    //    for (state=1;state<max_states_row;state++) {
    //		alpha_prime_row[state] = -MAXLOG;
    //	}
    memset(&alpha_prime_row[1], 0xCC, sizeof(float)*(max_states_row-1));

    // compute the beta_row matrix first, which is the reverse sweep (hence we start at the last
    // time instant-1 and work our way back to t=0).  we start at last time instant-1 b/c
    // we already filled in beta_row values for the last time instant, forcing the trellis to
    // start at state=0
//     DEBUG_PRINT("LL=%d KK=%d mm=%d\n", LL, rowEncoder_K, mm_row);
//     DEBUG_PRINT_F(fp, "LL=%d KK=%d mm=%d\n", LL, rowEncoder_K, mm_row);
    for (k=(LL+(rowEncoder_K-1)-1); k>=0; k--) {
        // TODO: figure out why this is needed? I'm still confused by this
        if (k<LL) {
            app_in = input_u_rows[k];
        }
        else {
            app_in = 0;
        }

        // get the input associated w/ this time instant
        memcpy(&rec_array_row[0], &input_c_rows[rowEncoder_n*k], sizeof(float)*rowEncoder_n);

//         DEBUG_PRINT("k=%d\n", k);
//         DEBUG_PRINT_F(fp, "k=%d\n", k);
//
//         DEBUG_PRINT("rec_array -->\n");
//         DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&rec_array_row[0], rec_array_row.size());
//         DEBUG_PRINT_F(fp, "rec_array -->\n");
//         DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &rec_array_row[0], rec_array_row.size());

        // for each input at this time instant, create a metric which
        // represents the likelihood that this input corresponds to
        // each of the possible symbols
        for(symbol=0; symbol<num_symbols_row; symbol++) {
            metric_c_row[symbol] = gamma(rec_array_row, symbol);
        }

//         DEBUG_PRINT("metric_c -->\n");
//         DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&metric_c_row[0], metric_c_row.size());
//         DEBUG_PRINT_F(fp, "metric_c -->\n");
//         DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &metric_c_row[0], metric_c_row.size());

        // step through all states -- populating the beta_row values for each node
        // in the trellis diagram as shown above w/ the maximum-likelihood
        // of this current node coming from the previous node
        for ( state=0; state< max_states_row; state++ ) {
            // data 0 branch
            delta1 = beta_row[k+1][rowNextStates[0][state]] + metric_c_row[rowOutputs[0][state]];
            // data 1 branch
            delta2 = beta_row[k+1][rowNextStates[1][state]] + metric_c_row[rowOutputs[1][state]] + app_in;
            // update beta_row
            beta_row[k][state] = (*this.*max_star)(delta1, delta2);

//             DEBUG_PRINT("delta1=%f delta2=%f beta=%f\n", delta1, delta2, beta_row[k][state]);
//             DEBUG_PRINT_F(fp, "delta1=%f delta2=%f beta=%f\n", delta1, delta2, beta_row[k][state]);
        }

        // normalize beta_row
        for (state=1;state<max_states_row;state++) {
        	beta_row[k][state] = beta_row[k][state] - beta_row[k][0];
        }
        beta_row[k][0] = 0;

    }

    // compute the forward sweep (alpha_row values), and update the llr's
    // notice that we start at time index=1, b/c time index=0 has already been
    // initialized, and we are forcing the trellis to start from state=0
    for(k=1; k<LL+rowEncoder_K; k++) {
        // initialize the llr's for the uncoded bits
        num_llr_u = -MAXLOG;
        den_llr_u = -MAXLOG;

        // initialize alpha_row
//        for (state=0;state<max_states_row;state++) {
//			alpha_row[state] = -MAXLOG;
//		}
        memset(&alpha_row[0], 0xCC, max_states_row*sizeof(float));

        // assign inputs
        if (k-1 < LL)
            app_in = input_u_rows[k-1];
        else
            app_in = 0;

        // initialize the llr's for the coded bits, and get the correct
        // input bits for this time instant
        for (ii=0;ii<rowEncoder_n;ii++) {
            den_llr_c_row[ii] = -MAXLOG;
            num_llr_c_row[ii] = -MAXLOG;
            rec_array_row[ii] = input_c_rows[rowEncoder_n*(k-1)+ii];
        }

        // for each input at this time instant, create a metric which
        // represents the likelihood that this input corresponds to
        // each of the possible symbols
        for(symbol=0; symbol<num_symbols_row; symbol++) {
            metric_c_row[ii] = gamma(rec_array_row, symbol);
        }

        // compute the alpha_row vector
        // to understand the loop below, we need to think about the forward trellis.
        // we know that any node, at any time instant in the trellis diagram can have
        // multiple transitions into that node (i.e. any number of nodes in the
        // previous time instance can transition into this current node, based on the
        // polynomial).  SO, in the loop below, delta1 represents the transition from
        // the previous time instant for input (either 0 or 1) and the current state
        // we are looping over, and delta2 represents a transition from the previous
        // time instant to the current time instant for input (either 0 or 1) that
        // we've already calculated.  Essentially, b/c we can have multiple possible
        // transitions into the current alpha_row node of interest, and we need to store the
        // maximum likelihood of all those transitions, we keep delta2 as a previously
        // calculated transition, and then take the max* of that, which can be thought
        // of as a maximum (in the MAX-LOG-MAP approximation)
        for ( state=0; state<max_states_row; state++ ) {
            // Data 0 branch
            delta1 = alpha_prime_row[state] + metric_c_row[rowOutputs[0][state]];
            delta2 = alpha_row[rowNextStates[0][state]];
            alpha_row[rowNextStates[0][state]] = (*this.*max_star)(delta1, delta2);

            // Data 1 branch
            delta1 = alpha_prime_row[state] + metric_c_row[rowOutputs[1][state]] + app_in;
            delta2 = alpha_row[rowNextStates[1][state]];
            alpha_row[rowNextStates[1][state]] = (*this.*max_star)(delta1, delta2);
        }

        // compute the llr's
        for (state=0;state<max_states_row;state++)  {
            // data 0 branch (departing)
            delta1 = alpha_prime_row[state] + metric_c_row[rowOutputs[0][state]] + beta_row[k][rowNextStates[0][state]];
            // the information bit
            delta2 = den_llr_u;
            den_llr_u = (*this.*max_star)(delta1, delta2);
            mask = 1<<(rowEncoder_n-1);
            // go through all the code bits
            for (ii=0;ii<rowEncoder_n;ii++) {
                if ( rowOutputs[0][state]&mask ) {
                    // this code bit 1
                    delta2 = num_llr_c_row[ii];
                    num_llr_c_row[ii] = (*this.*max_star)(delta1, delta2);
                } else {
                    // this code bit is 0
                    delta2 = den_llr_c_row[ii];
                    den_llr_c_row[ii] = (*this.*max_star)(delta1, delta2);
                }
                mask = mask>>1;
            }

            // data 1 branch (departing)
            delta1 = alpha_prime_row[state] + metric_c_row[rowOutputs[1][state]] + beta_row[k][rowNextStates[1][state]] + app_in;
            // the information bit
            delta2 = num_llr_u;
            num_llr_u = (*this.*max_star)(delta1, delta2);
            mask = 1<<(rowEncoder_n-1);
            // go through all the code bits
            for (ii=0;ii<rowEncoder_n;ii++) {
                if ( rowOutputs[1][state]&mask ) {
                    // this code bit 1
                    delta2 = num_llr_c_row[ii];
                    num_llr_c_row[ii] = (*this.*max_star)(delta1, delta2);
                } else {
                    // this code bit is 0
                    delta2 = den_llr_c_row[ii];
                    den_llr_c_row[ii] = (*this.*max_star)(delta1, delta2);
                }
                mask = mask>>1;
            }

            // shift alpha_row back to alpha_prime_row
            alpha_prime_row[state] = alpha_row[state] - alpha_row[0];
        }

        // assign uncoded outputs
        if (k-1<LL) {
            output_u_rows[k-1] = num_llr_u - den_llr_u;
        }
        // assign coded outputs
        volk_32f_x2_subtract_32f(&output_c_rows[rowEncoder_n*(k-1)], &num_llr_c_row[0], &den_llr_c_row[0], rowEncoder_n);
    }
}

void tpc_decoder::siso_decode_col() {

    int LL, state, k, ii, symbol, mask;
    float app_in, delta1, delta2;
    LL = input_u_cols.size();        // code length

    // log-likelihood ratio of the uncoded bit being a 1
    float num_llr_u;
    // log-likelihood ratio of the uncoded bit being a 0
    float den_llr_u;

    // initialize beta_col trellis
    // this initialization is saying that the likelihood that the reverse sweep
    // starts at state=0 is 100%, b/c state 1, 2, 3's likelihood's are basically -inf
    // this implies that the forward trellis terminated at the 0 state
//    for(state=1; state<max_states_col; state++) {
//		beta_col[LL+colEncoder_K-1][state] = -MAXLOG;
//	}
    memset(&beta_col[LL+colEncoder_K-1][1], 0xCC, sizeof(float)*(max_states_col-1));

    // initialize alpha_prime_col (current time instant), alpha_prime_col then gets updated
    // by shifting in alpha_col at the end of each time instant of processing
    // alpha_col needs to get initialized at the beginning of each processing loop, so we
    // initialize alpha_col in the forward sweep processing loop
    // the initialization below is saying that the likelihood of starting at state=0 is
    // 100%, b/c state 1, 2, 3's likelihoods are basically -inf
    // as with the beta_col array, this implies that the forward trellis started at state=0
//    for (state=1;state<max_states_col;state++) {
//		alpha_prime_col[state] = -MAXLOG;
//	}
    memset(&alpha_prime_col[1], 0xCC, sizeof(float)*(max_states_col-1));

    // compute the beta_col matrix first, which is the reverse sweep (hence we start at the last
    // time instant-1 and work our way back to t=0).  we start at last time instant-1 b/c
    // we already filled in beta_col values for the last time instant, forcing the trellis to
    // start at state=0
//     DEBUG_PRINT("LL=%d KK=%d mm=%d\n", LL, colEncoder_K, mm_col);
//     DEBUG_PRINT_F(fp, "LL=%d KK=%d mm=%d\n", LL, colEncoder_K, mm_col);
    for (k=(LL+(colEncoder_K-1)-1); k>=0; k--) {
        // TODO: figure out why this is needed? I'm still confused by this
        if (k<LL) {
            app_in = input_u_cols[k];
        }
        else {
            app_in = 0;
        }

        // get the input associated w/ this time instant
        memcpy(&rec_array_col[0], &input_c_cols[colEncoder_n*k], sizeof(float)*colEncoder_n);

//         DEBUG_PRINT("k=%d\n", k);
//         DEBUG_PRINT_F(fp, "k=%d\n", k);
//
//         DEBUG_PRINT("rec_array -->\n");
//         DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&rec_array[0], rec_array_col.size());
//         DEBUG_PRINT_F(fp, "rec_array -->\n");
//         DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &rec_array[0], rec_array_col.size());

        // for each input at this time instant, create a metric which
        // represents the likelihood that this input corresponds to
        // each of the possible symbols
        for(symbol=0; symbol<num_symbols_col; symbol++) {
            metric_c_col[symbol] = gamma(rec_array_col, symbol);
        }

//         DEBUG_PRINT("metric_c -->\n");
//         DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&metric_c_col[0], metric_c_col.size());
//         DEBUG_PRINT_F(fp, "metric_c -->\n");
//         DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &metric_c_col[0], metric_c_col.size());

        // step through all states -- populating the beta_col values for each node
        // in the trellis diagram as shown above w/ the maximum-likelihood
        // of this current node coming from the previous node
        for ( state=0; state< max_states_col; state++ ) {
            // data 0 branch
        	delta1 = beta_col[k+1][colNextStates[0][state]] + metric_c_col[colOutputs[0][state]];
            // data 1 branch
            delta2 = beta_col[k+1][colNextStates[1][state]] + metric_c_col[colOutputs[1][state]] + app_in;
            // update beta_col
            beta_col[k][state] = (*this.*max_star)(delta1, delta2);

//             DEBUG_PRINT("delta1=%f delta2=%f beta=%f\n", delta1, delta2, beta_col[k][state]);
//             DEBUG_PRINT_F(fp, "delta1=%f delta2=%f beta=%f\n", delta1, delta2, beta_col[k][state]);
        }

        // normalize beta_col
        for (state=1;state<max_states_col;state++) {
        	beta_col[k][state] = beta_col[k][state] - beta_col[k][0];
        }
        beta_col[k][0] = 0;

    }

    // compute the forward sweep (alpha_col values), and update the llr's
    // notice that we start at time index=1, b/c time index=0 has already been
    // initialized, and we are forcing the trellis to start from state=0
    for(k=1; k<LL+colEncoder_K; k++) {
        // initialize the llr's for the uncoded bits
        num_llr_u = -MAXLOG;
        den_llr_u = -MAXLOG;

        // initialize alpha_col
//        for (state=0;state<max_states_col;state++) {
//			alpha_col[state] = -MAXLOG;
//		}
        memset(&alpha_col[0], 0xCC, max_states_col*sizeof(float));

        // assign inputs
        if (k-1 < LL)
            app_in = input_u_cols[k-1];
        else
            app_in = 0;

        // initialize the llr's for the coded bits, and get the correct
        // input bits for this time instant
        for (ii=0;ii<colEncoder_n;ii++) {
            den_llr_c_col[ii] = -MAXLOG;
            num_llr_c_col[ii] = -MAXLOG;
            rec_array_col[ii] = input_c_cols[colEncoder_n*(k-1)+ii];
        }

        // for each input at this time instant, create a metric which
        // represents the likelihood that this input corresponds to
        // each of the possible symbols
        for(symbol=0; symbol<num_symbols_col; symbol++) {
            metric_c_col[ii] = gamma(rec_array_col, symbol);
        }

        // compute the alpha_col vector
        // to understand the loop below, we need to think about the forward trellis.
        // we know that any node, at any time instant in the trellis diagram can have
        // multiple transitions into that node (i.e. any number of nodes in the
        // previous time instance can transition into this current node, based on the
        // polynomial).  SO, in the loop below, delta1 represents the transition from
        // the previous time instant for input (either 0 or 1) and the current state
        // we are looping over, and delta2 represents a transition from the previous
        // time instant to the current time instant for input (either 0 or 1) that
        // we've already calculated.  Essentially, b/c we can have multiple possible
        // transitions into the current alpha_col node of interest, and we need to store the
        // maximum likelihood of all those transitions, we keep delta2 as a previously
        // calculated transition, and then take the max* of that, which can be thought
        // of as a maximum (in the MAX-LOG-MAP approximation)
        for ( state=0; state<max_states_col; state++ ) {
            // Data 0 branch
            delta1 = alpha_prime_col[state] + metric_c_col[colOutputs[0][state]];
            delta2 = alpha_col[colNextStates[0][state]];
            alpha_col[colNextStates[0][state]] = (*this.*max_star)(delta1, delta2);

            // Data 1 branch
            delta1 = alpha_prime_col[state] + metric_c_col[colOutputs[1][state]] + app_in;
            delta2 = alpha_col[colNextStates[1][state]];
            alpha_col[colNextStates[1][state]] = (*this.*max_star)(delta1, delta2);
        }

        // compute the llr's
        for (state=0;state<max_states_col;state++)  {
            // data 0 branch (departing)
            delta1 = alpha_prime_col[state] + metric_c_col[colOutputs[0][state]] + beta_col[k][colNextStates[0][state]];
            // the information bit
            delta2 = den_llr_u;
            den_llr_u = (*this.*max_star)(delta1, delta2);
            mask = 1<<(colEncoder_n-1);
            // go through all the code bits
            for (ii=0;ii<colEncoder_n;ii++) {
                if ( colOutputs[0][state]&mask ) {
                    // this code bit 1
                    delta2 = num_llr_c_col[ii];
                    num_llr_c_col[ii] = (*this.*max_star)(delta1, delta2);
                } else {
                    // this code bit is 0
                    delta2 = den_llr_c_col[ii];
                    den_llr_c_col[ii] = (*this.*max_star)(delta1, delta2);
                }
                mask = mask>>1;
            }

            // data 1 branch (departing)
            delta1 = alpha_prime_col[state] + metric_c_col[colOutputs[1][state]] + beta_col[k][colNextStates[1][state]] + app_in;
            // the information bit
            delta2 = num_llr_u;
            num_llr_u = (*this.*max_star)(delta1, delta2);
            mask = 1<<(colEncoder_n-1);
            // go through all the code bits
            for (ii=0;ii<colEncoder_n;ii++) {
                if ( colOutputs[1][state]&mask ) {
                    // this code bit 1
                    delta2 = num_llr_c_col[ii];
                    num_llr_c_col[ii] = (*this.*max_star)(delta1, delta2);
                } else {
                    // this code bit is 0
                    delta2 = den_llr_c_col[ii];
                    den_llr_c_col[ii] = (*this.*max_star)(delta1, delta2);
                }
                mask = mask>>1;
            }

            // shift alpha_col back to alpha_prime_col
            alpha_prime_col[state] = alpha_col[state] - alpha_col[0];
        }

        // assign uncoded outputs
        if (k-1<LL) {
            output_u_cols[k-1] = num_llr_u - den_llr_u;
        }
        // assign coded outputs
        volk_32f_x2_subtract_32f(&output_c_cols[output_c_col_idx+rowEncoder_n*(k-1)], &num_llr_c_col[0], &den_llr_c_col[0], colEncoder_n);
    }
}

float tpc_decoder::linear_log_map(const float delta1, const float delta2) {
    float diff;

    diff = delta2 - delta1;

    if ( diff > TJIAN )
        return( delta2 );
    else if ( diff < -TJIAN )
        return( delta1 );
    else if ( diff > 0 )
        return( delta2 + AJIAN*(diff-TJIAN) );
    else
        return( delta1 - AJIAN*(diff+TJIAN) );
}

// MAX-LOG-MAP approximation
float tpc_decoder::max_log_map(const float delta1, const float delta2) {
    if(delta1>delta2) return delta1;
    return delta2;
}

float tpc_decoder::constant_log_map(const float delta1, const float delta2) {
    // Return maximum of delta1 and delta2
    // and in correction value if |delta1-delta2| < TVALUE
    register float diff;
    diff = delta2 - delta1;

    if ( diff > TVALUE )
        return( delta2 );
    else if ( diff < -TVALUE )
        return( delta1 );
    else if ( diff > 0 )
        return( delta2 + CVALUE );
    else
        return( delta1 + CVALUE );
}

float tpc_decoder::log_map_lut_correction(const float delta1, const float delta2) {
    float diff;
    diff = (float) fabs( delta2 - delta1 );

    if (delta1 > delta2) {
        if (diff > BOUNDARY8 )
            return( delta1 );
        else if ( diff > BOUNDARY4 ) {
            if (diff > BOUNDARY6 ) {
                if ( diff > BOUNDARY7 )
                    return( delta1 + VALUE7 + SLOPE7*(diff-BOUNDARY7) );
                else
                    return( delta1 + VALUE6 + SLOPE6*(diff-BOUNDARY6) );
            } else {
                if ( diff > BOUNDARY5 )
                    return( delta1 + VALUE5 + SLOPE5*(diff-BOUNDARY5) );
                else
                    return( delta1 + VALUE4 + SLOPE4*(diff-BOUNDARY4) );
            }
        } else {
            if (diff > BOUNDARY2 ) {
                if ( diff > BOUNDARY3 )
                    return( delta1 + VALUE3 + SLOPE3*(diff-BOUNDARY3) );
                else
                    return( delta1 + VALUE2 + SLOPE2*(diff-BOUNDARY2) );
            } else {
                if ( diff > BOUNDARY1 )
                    return( delta1 + VALUE1 + SLOPE1*(diff-BOUNDARY1) );
                else
                    return( delta1 + VALUE0 + SLOPE0*(diff-BOUNDARY0) );
            }
        }
    } else {
        if (diff > BOUNDARY8 )
            return( delta2 );
        else if ( diff > BOUNDARY4 ) {
            if (diff > BOUNDARY6 ) {
                if ( diff > BOUNDARY7 )
                    return( delta2 + VALUE7 + SLOPE7*(diff-BOUNDARY7) );
                else
                    return( delta2 + VALUE6 + SLOPE6*(diff-BOUNDARY6) );
            } else {
                if ( diff > BOUNDARY5 )
                    return( delta2 + VALUE5 + SLOPE5*(diff-BOUNDARY5) );
                else
                    return( delta2 + VALUE4 + SLOPE4*(diff-BOUNDARY4) );
            }
        } else {
            if (diff > BOUNDARY2 ) {
                if ( diff > BOUNDARY3 )
                    return( delta2 + VALUE3 + SLOPE3*(diff-BOUNDARY3) );
                else
                    return( delta2 + VALUE2 + SLOPE2*(diff-BOUNDARY2) );
            } else {
                if ( diff > BOUNDARY1 )
                    return( delta2 + VALUE1 + SLOPE1*(diff-BOUNDARY1) );
                else
                    return( delta2 + VALUE0 + SLOPE0*(diff-BOUNDARY0) );
            }
        }
    }
}

float tpc_decoder::log_map_cfunction_correction(const float delta1, const float delta2) {
    // Use C-function calls to compute the correction function
    if (delta1 > delta2) {
        return( (float) (delta1 + log( 1 + exp( delta2-delta1) ) ) );
    } else  {
        return( (float) (delta2 + log( 1 + exp( delta1-delta2) ) ) );
    }
}

float tpc_decoder::gamma(const std::vector<float> rx_array, const int symbol) {
    float rm = 0;
    int ii;
    int mask;
    int nn = rx_array.size();

    mask = 1;
    for (ii=0;ii<nn;ii++) {
        if (symbol&mask)
            rm += rx_array[nn-ii-1];
        mask = mask<<1;
    }

//     DEBUG_PRINT("nn=%d symbol=%d rm = %f\n", nn, symbol, rm);
//     DEBUG_PRINT_F(fp, "nn=%d symbol=%d rm = %f\n", nn, symbol, rm);

    return(rm);
}

template <typename T> int tpc_decoder::sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void tpc_decoder::generic_work(void *inBuffer, void *outBuffer) {
    const float *inPtr = (const float *) inBuffer;
    unsigned char *out = (unsigned char *) outBuffer;

    unsigned int m, n, ii;
    int iter;

    for(ii=0; ii<numInitLoadIter; ii++) {
    	memset(&channel_llr[ii][0], 0, sizeof(float)*codeword_N);
    	memset(&Z[ii][0], 0, sizeof(float)*codeword_N);
    }
    memset(&channel_llr[ii][0], 0, sizeof(float)*numInitRemaining);
    memset(&Z[ii][0], 0, sizeof(float)*numInitRemaining);
    memcpy(&channel_llr[ii][numInitRemaining], &inPtr[0], (codeword_N-numInitRemaining)*sizeof(float));
    memcpy(&Z[ii][numInitRemaining], &inPtr[0], (codeword_N-numInitRemaining)*sizeof(float));
    int inPtrIdx = codeword_N-numInitRemaining;
    for(ii=ii+1; ii<codeword_M; ii++) {
		memcpy(&channel_llr[ii][0], &inPtr[inPtrIdx], sizeof(float)*codeword_N);
		memcpy(&Z[ii][0], &inPtr[inPtrIdx], sizeof(float)*codeword_N);
		inPtrIdx += codeword_N;
	}

    // clear out extrinsic-info between blocks
    memset(&extrinsic_info[0], 0, sizeof(float)*extrinsic_info.size());

    //DEBUG_PRINT("Starting TURBO Decoding\n");

    for(iter=0; iter<d_max_iter; iter++) {
        //DEBUG_PRINT("Turbo Iter=%d\n", iter+1);
        //DEBUG_PRINT_F(fp, "Turbo Iter=%d\n", iter+1);
        // decode each row
        for(m=0; m<codeword_M; m++) {
            // stage the data
        	volk_32f_x2_add_32f(&input_c_rows[0], &channel_llr[m][0], &extrinsic_info[m*codeword_N], codeword_N);

            //DEBUG_PRINT("input_c_rows -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&input_c_rows[0], codeword_N);
            //DEBUG_PRINT_F(fp, "input_c_rows -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &input_c_rows[0], codeword_N);

            // call siso decode
            siso_decode_row();

            //DEBUG_PRINT("output_u_rows -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&output_u_rows[0], output_u_rows.size());
            //DEBUG_PRINT_F(fp, "output_u_rows -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &output_u_rows[0], output_u_rows.size());
            //DEBUG_PRINT("output_c_rows -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&output_c_rows[0], output_c_rows.size());
            //DEBUG_PRINT_F(fp, "output_c_rows -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &output_c_rows[0], output_c_rows.size());

            // copy the output coded back into Z, so we can feed it back through the decoder
            // for more iterations
            volk_32f_x2_subtract_32f(&Z[m][0], &output_c_rows[0], &extrinsic_info[m*codeword_N], codeword_N);
        }

        // decode each col
        earlyExit = true;
        output_c_col_idx = 0;
        for(n=0; n<codeword_N; n++) {
            // stage the data
            for(ii=0; ii<codeword_M; ii++) {
                input_c_cols[ii] = Z[ii][n];
            }

            //DEBUG_PRINT("input_c_cols -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&input_c_cols[0], codeword_M);
            //DEBUG_PRINT_F(fp, "input_c_cols -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &input_c_cols[0], codeword_M);

            // call siso decode
            siso_decode_col();

            //DEBUG_PRINT("output_u_cols -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&output_u_cols[0], output_u_cols.size());
            //DEBUG_PRINT_F(fp, "output_u_cols -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &output_u_cols[0], output_u_cols.size());
            //DEBUG_PRINT("output_c_cols -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT(&output_c_cols[output_c_col_idx], codeword_M);
            //DEBUG_PRINT_F(fp, "output_c_cols -->\n");
            //DEBUG_PRINT_FLOAT_ARRAY_AS_FLOAT_F(fp, &output_c_cols[output_c_col_idx], codeword_M);

            // create the extrinsic_info vector, which subtracts from input_c_cols to prevent feedback
            //DEBUG_PRINT("extrinsic_info -->\n");
            //DEBUG_PRINT_F(fp, "extrinsic_info -->\n");
            for(ii=0; ii<codeword_M; ii++) {
            	extrinsic_info[ii*codeword_N+n] = output_c_cols[output_c_col_idx+ii] - input_c_cols[ii];

            	if(earlyExit) {
            		if(sgn(output_c_cols[output_c_col_idx+ii])!=sgn(input_c_cols[ii])) {
						earlyExit = false;
					}
            	}


                //DEBUG_PRINT("%f ", extrinsic_info[ii*codeword_N+n]);
                //DEBUG_PRINT_F(fp, "%f ", extrinsic_info[ii*codeword_N+n]);
            }
            //DEBUG_PRINT("\n");
            //DEBUG_PRINT_F(fp, "\n");

            output_c_col_idx += codeword_M;
        }
        if(earlyExit) break;
    }

    ii=0;
	for(m=0; m<d_kcol; m++) {
		for(n=0; n<d_krow; n++) {
			if(ii==0) {
				m = mInit;
				n = nInit;
			}

			// make a hard decision
//			if(output_matrix[n*codeword_M+m]>0) {
			if(output_c_cols[n*codeword_M+m]>0) {
				out[ii++] = (unsigned char) 1;
			}
			else {
				out[ii++] = (unsigned char) 0;
			}
		}
	}

    //DEBUG_PRINT(("Output\n"));
    //DEBUG_PRINT_UCHAR_ARRAY(out, outputSize);
    //DEBUG_PRINT_F(fp, "Output\n");
    //DEBUG_PRINT_UCHAR_ARRAY_F(fp, out, outputSize);
}

int tpc_decoder::get_input_item_size() {
    return sizeof(INPUT_DATATYPE);
}

int tpc_decoder::get_output_item_size() {
    return sizeof(OUTPUT_DATATYPE);
}

int tpc_decoder::get_history() {
    return 0;
}

float tpc_decoder::get_shift() {
    return 0.0;
}

const char* tpc_decoder::get_conversion() {
    return "none";
}

tpc_decoder::~tpc_decoder()
{
  if(fp)
    fclose(fp);
}

}
}
