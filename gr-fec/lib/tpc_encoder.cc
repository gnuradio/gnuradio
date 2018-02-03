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

#include <gnuradio/fec/tpc_encoder.h>
#include <gnuradio/fec/tpc_common.h>
#include <gnuradio/fec/generic_encoder.h>

#include <math.h>
#include <boost/assign/list_of.hpp>
#include <volk/volk.h>
#include <sstream>
#include <stdio.h>
#include <vector>

#include <algorithm>            // for std::reverse
#include <string.h>             // for memcpy

namespace gr {
namespace fec {
generic_encoder::sptr
tpc_encoder::make(std::vector<int> row_polys, std::vector<int> col_polys, int krow, int kcol, int bval, int qval)
{
    return generic_encoder::sptr(new tpc_encoder(row_polys, col_polys, krow, kcol, bval, qval));
}

tpc_encoder::tpc_encoder (std::vector<int> row_polys, std::vector<int> col_polys, int krow, int kcol, int bval, int qval)
    : d_rowpolys(row_polys), d_colpolys(col_polys), d_krow(krow), d_kcol(kcol),
      d_bval(bval), d_qval(qval)
{
    // first we operate on data chunks of get_input_size()
    // TODO: should we verify this and throw an error if it doesn't match?  YES
    // hwo do we do that?

    // calculate the input and output sizes
    inputSize = (d_krow*d_kcol - (d_bval+d_qval));
    rowEncoder_K = ceil(log(d_rowpolys[0])/log(2));    // rowEncoder_K is the constraint length of the row encoder polynomial
    rowEncoder_n = d_rowpolys.size();
    rowEncoder_m = rowEncoder_K - 1;
    colEncoder_K = ceil(log(d_colpolys[0])/log(2));    // colEncoder_K is the constraint length of the col encoder polynomial
    colEncoder_n = d_colpolys.size();
    colEncoder_m = colEncoder_K - 1;

    outputSize = ((d_krow+rowEncoder_m)*rowEncoder_n)*((d_kcol+colEncoder_m)*colEncoder_n) - d_bval;

    fp = NULL;
    //DEBUG_PRINT("inputSize=%d outputSize=%d\n", inputSize, outputSize);
    //fp = fopen("c_encoder_output.txt", "w");

    // resize internal matrices
    rowNumStates = 1 << (rowEncoder_K-1);       // 2^(row_mm)
    colNumStates = 1 << (colEncoder_K-1);       // 2^(col_mm)
    rowOutputs.resize(2, std::vector<int>(rowNumStates,0));
    rowNextStates.resize(2, std::vector<int>(rowNumStates,0));
    colOutputs.resize(2, std::vector<int>(colNumStates,0));
    colNextStates.resize(2, std::vector<int>(colNumStates,0));;

    rowTail.resize(rowNumStates, 0);
    colTail.resize(colNumStates, 0);

    // precalculate the state transition matrix for the row polynomial
    tpc_common::precomputeStateTransitionMatrix_RSCPoly(rowNumStates, d_rowpolys, rowEncoder_K, rowEncoder_n, 
                                                        rowOutputs, rowNextStates);

    // calculate the tail for the row
    tpc_common::rsc_tail(rowTail, d_rowpolys, rowNumStates, rowEncoder_m);

    // precalculate the state transition matrix for the column polynomial
    tpc_common::precomputeStateTransitionMatrix_RSCPoly(colNumStates, d_colpolys, colEncoder_K, colEncoder_n, 
                                                        colOutputs, colNextStates);
    // calculate the tail for the col
    tpc_common::rsc_tail(colTail, d_colpolys, colNumStates, colEncoder_m);

    // pre-allocate memory we use for encoding
    inputSizeWithPad = d_bval+d_qval+inputSize;
    inputWithPad.resize(inputSizeWithPad, 0);

    numRowsToEncode = inputSizeWithPad/d_krow;      // this should be OK w/ integer division -- TODO: check this?
    rowToEncode.resize(d_krow,0);
    rowEncoded_block.resize(d_krow+(rowEncoder_m*rowEncoder_n), 0);
    rowEncodedBits.resize(d_kcol, std::vector<uint8_t>(rowEncoder_m*rowEncoder_n,0) );

    numColsToEncode = d_krow+(rowEncoder_m*rowEncoder_n);
    colToEncode.resize(d_kcol,0);
    colEncoded_block.resize(d_kcol+(colEncoder_m*colEncoder_n), 0);
    colEncodedBits.resize(d_krow+(rowEncoder_m*rowEncoder_n), std::vector<uint8_t>(colEncoder_m*colEncoder_n,0) );
}

int tpc_encoder::get_output_size() {
    return outputSize;
}

int tpc_encoder::get_input_size() {
    return inputSize;
}

void tpc_encoder::block_conv_encode( std::vector<uint8_t> &output,
                               std::vector<uint8_t> input,
                               std::vector< std::vector<int> > transOutputVec,
                               std::vector< std::vector<int> > transNextStateVec,
                               std::vector<int> tail,
                               size_t KK,
                               size_t nn)
{
    size_t outsym, ii, jj;
    int state = 0;
    size_t LL = input.size();

    std::vector<int> binVec(nn,0);

    // encode data bits one bit at a time
    for (ii=0; ii<LL; ii++) {

        // determine the output symbol
        outsym = transOutputVec[(int)input[ii]][state];
        // determine the next state
        state = transNextStateVec[(int)input[ii]][state];

        // Convert symbol to a binary vector
        tpc_common::itob( binVec, outsym, nn );

        // Assign to output : TODO: investigate using memcpy for this?
        for (jj=0;jj<nn;jj++) {
            output[nn*ii+jj] = binVec[jj];
        }
    }

    // encode tail
    for (ii=LL;ii<LL+KK-1;ii++) {

        // determine the output symbol
        outsym = transOutputVec[tail[state]][state];
        // determine the next state
        state = transNextStateVec[tail[state]][state];

        // Convert symbol to a binary vector
        tpc_common::itob( binVec, outsym, nn );

        // Assign to output : TODO: investigate using memcpy for this?
        for (jj=0;jj<nn;jj++) {
            output[nn*ii+jj] = binVec[jj];
        }
    }
}

void tpc_encoder::generic_work(void *inBuffer, void *outBuffer) {
    const uint8_t *in = (const uint8_t *) inBuffer;
    uint8_t *out = (uint8_t *) outBuffer;

    size_t ii, jj;     // indexing var

    //DEBUG_PRINT_UCHAR_ARRAY(in, inputSize);

    // TODO: probably a better way to do this than memcpy?
    memcpy(&inputWithPad[d_bval+d_qval], in, sizeof(unsigned char)*inputSize);

    //DEBUG_PRINT("Input with Pad -->\n");
    //DEBUG_PRINT_UCHAR_ARRAY(&inputWithPad[0], inputSizeWithPad);
    //DEBUG_PRINT_F(fp, "Input with Pad -->\n");
    //DEBUG_PRINT_UCHAR_ARRAY_F(fp, &inputWithPad[0], inputSizeWithPad);

    // encode the row data
    for(ii=0; ii<numRowsToEncode; ii++) {
        // populate rowToEncode
        memcpy(&rowToEncode[0], &inputWithPad[ii*d_krow], sizeof(unsigned char)*d_krow);

        //DEBUG_PRINT("Encoding row=[%d] -->\n",ii);
        //DEBUG_PRINT_UCHAR_ARRAY(&rowToEncode[0], d_krow);
        //DEBUG_PRINT_F(fp, "Encoding row=[%d] -->\n",ii);
        //DEBUG_PRINT_UCHAR_ARRAY_F(fp, &rowToEncode[0], d_krow);

        // encode it
        block_conv_encode( rowEncoded_block,
                           rowToEncode,
                           rowOutputs,
                           rowNextStates,
                           rowTail,
                           rowEncoder_K,
                           rowEncoder_n);

        //DEBUG_PRINT("Row Encoded Block=[%d] -->\n",ii);
        //DEBUG_PRINT_FLOAT_ARRAY_AS_UCHAR(&rowEncoded_block[0], tmp);
        //DEBUG_PRINT_F(fp, "Row Encoded Block=[%d] -->\n",ii);
        //DEBUG_PRINT_FLOAT_ARRAY_AS_UCHAR_F(fp, &rowEncoded_block[0], tmp);

        // store only the encoded bits, b/c we read out the data in a special way
        memcpy(&rowEncodedBits[ii][0], &rowEncoded_block[d_krow], sizeof(uint8_t)*(rowEncoder_m*rowEncoder_n));

//         DEBUG_PRINT("Row Encoded Bits");
//         tmp = rowEncoder_m*rowEncoder_n;
//         DEBUG_PRINT_FLOAT_ARRAY_AS_UCHAR(&rowEncodedBits[ii][0], tmp);
    }

    // encode the column data
    size_t numDataColsToEncode = d_krow;
    size_t numCheckColsToEncode = numColsToEncode-numDataColsToEncode;
    for(ii=0; ii<numDataColsToEncode; ii++) {
        // populate colToEncode
        for(jj=0; jj<d_kcol; jj++) {
            colToEncode[jj] = inputWithPad[jj*d_krow+ii];
        }

        //DEBUG_PRINT("Encoding col=[%d] -->\n",ii);
        //DEBUG_PRINT_UCHAR_ARRAY(&colToEncode[0], d_kcol);
        //DEBUG_PRINT_F(fp, "Encoding col=[%d] -->\n",ii);
        //DEBUG_PRINT_UCHAR_ARRAY_F(fp, &colToEncode[0], d_kcol);

        // encode it
        block_conv_encode( colEncoded_block,
                           colToEncode,
                           colOutputs,
                           colNextStates,
                           colTail,
                           colEncoder_K,
                           colEncoder_n);

        //DEBUG_PRINT("Col Encoded Block=[%d] -->\n",ii);
        //DEBUG_PRINT_FLOAT_ARRAY_AS_UCHAR(&colEncoded_block[0], tmp);
        //DEBUG_PRINT_F(fp, "Col Encoded Block=[%d] -->\n",ii);
        //DEBUG_PRINT_FLOAT_ARRAY_AS_UCHAR_F(fp, &colEncoded_block[0], tmp);

        // store only the encoded bits, b/c we read the data out in a special way
        memcpy(&colEncodedBits[ii][0], &colEncoded_block[d_kcol], sizeof(uint8_t)*(colEncoder_m*colEncoder_n));

//         DEBUG_PRINT("Col Encoded Bits");
//         tmp = colEncoder_m*colEncoder_n;
//         DEBUG_PRINT_FLOAT_ARRAY(&colEncodedBits[ii][0], tmp);
    }

    // encode checks on checks (encode the row-encoded bits)
    for(ii=0; ii<numCheckColsToEncode; ii++) {
        // populate colToEncode
        for(jj=0; jj<d_kcol; jj++) {
            colToEncode[jj] = rowEncodedBits[jj][ii];       // indexing is weird b/c of the way we declared the vector :(
        }

        //DEBUG_PRINT("Encoding col=[%d] -->\n",ii+numDataColsToEncode);
        //DEBUG_PRINT_UCHAR_ARRAY(&colToEncode[0], d_kcol);
        //DEBUG_PRINT_F(fp, "Encoding col=[%d] -->\n",ii+numDataColsToEncode);
        //DEBUG_PRINT_UCHAR_ARRAY_F(fp, &colToEncode[0], d_kcol);

        // encode it
        block_conv_encode( colEncoded_block,
                           colToEncode,
                           colOutputs,
                           colNextStates,
                           colTail,
                           colEncoder_K,
                           colEncoder_n);

        //DEBUG_PRINT("Col Encoded Block=[%d] -->\n",ii+numDataColsToEncode);
        //DEBUG_PRINT_FLOAT_ARRAY_AS_UCHAR(&colEncoded_block[0], tmp);

        //DEBUG_PRINT_F(fp, "Col Encoded Block=[%d] -->\n",ii+numDataColsToEncode);
        //DEBUG_PRINT_FLOAT_ARRAY_AS_UCHAR_F(fp, &colEncoded_block[0], tmp);

        // store only the encoded bits, b/c we read the data out in a special way
        memcpy(&colEncodedBits[ii+numDataColsToEncode][0], &colEncoded_block[d_kcol], sizeof(uint8_t)*(colEncoder_m*colEncoder_n));

//         DEBUG_PRINT("Col Encoded Bits");
//         tmp = colEncoder_m*colEncoder_n;
//         DEBUG_PRINT_FLOAT_ARRAY(&colEncodedBits[ii][0], tmp);
    }

    unsigned char* inputDataPtr;
    uint8_t *outputDataPtr = out;

    int curRowInRowEncodedBits = 0;
    // read out the data along the rows into the "out" array

    // skip B zeros & do the first row
    inputDataPtr =  &inputWithPad[d_bval];
    if(d_bval > d_krow){ throw std::runtime_error("bval must be < krow"); }
    size_t firstRowRemainingBits = d_krow-d_bval;
    for(ii=0; ii<firstRowRemainingBits; ii++) {
        *outputDataPtr++ = (uint8_t)(*inputDataPtr++);
    }

    // copy the encoded bits
    memcpy(outputDataPtr, &rowEncodedBits[curRowInRowEncodedBits++][0],
            sizeof(uint8_t)*(rowEncoder_m*rowEncoder_n));

    outputDataPtr += (rowEncoder_m*rowEncoder_n);

    // copy out the rest of the data
    for(ii=1; ii<d_kcol; ii++) {        // ii starts at 1, b/c we already did idx=0
        // copy systematic bits
        for(jj=0; jj<d_krow; jj++) {
            *outputDataPtr++ = (uint8_t)(*inputDataPtr++);
        }

        // copy the encoded bits
        memcpy(outputDataPtr, &rowEncodedBits[curRowInRowEncodedBits++][0],
               sizeof(uint8_t)*(rowEncoder_m*rowEncoder_n));

        outputDataPtr += (rowEncoder_m*rowEncoder_n);
    }

    // copy the encoded cols
    for(ii=0; ii<(colEncoder_m*colEncoder_n); ii++) {
        // copy checks
        for(jj=0; jj<d_krow; jj++) {
            *outputDataPtr++ = colEncodedBits[jj][ii];
        }
        int kk = jj;
        // copy checks on checks
        for(jj=0; jj<(rowEncoder_m*rowEncoder_n); jj++) {
            *outputDataPtr++ = colEncodedBits[kk++][ii];
        }
    }

    //DEBUG_PRINT("Output\n");
    //DEBUG_PRINT_FLOAT_ARRAY_AS_UCHAR(out, outputSize);
    //DEBUG_PRINT_F(fp, "Output\n");
    //DEBUG_PRINT_FLOAT_ARRAY_AS_UCHAR_F(fp, out, outputSize);
}

tpc_encoder::~tpc_encoder()
{
    if(fp)
      fclose(fp);
}

}
}
