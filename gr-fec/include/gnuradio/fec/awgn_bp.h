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

/* -----------------------------------------------------------------
 *
 * This class defines functions for message passing mechanism for a
 * AWGN channel. Message passing (also known as belief propagation)
 * is used for decoding LDPC codes. Details of how LDPC codes are
 * decoded is available in the link below
 * - http://www.cs.utoronto.ca/~radford/ftp/LDPC-2012-02-11/decoding.html
 *
 * Belief propagation decoding is a suboptimal but efficient method of
 * decoding LDPC codes.
 *
 */

#ifndef AWGN_BP_H
#define AWGN_BP_H

#include <vector>
#include <cmath>
#include <iostream>
#include "gf2mat.h"
#include "alist.h"
#include <gnuradio/fec/api.h>

class FEC_API awgn_bp
{
  public:
    //! Default constructor
    awgn_bp () {};

    //! A constructor for given GF2Mat and sigma
    awgn_bp (const GF2Mat  X, float sgma);

    //! A constructor for given alist and sigma
    awgn_bp (alist  _list, float sgma);

    //! Initializes the class using given alist and sigma
    void set_alist_sigma(alist  _list, float sgma);

    //! Returns the variable Q
    std::vector< std::vector<double> > get_Q();

    //! Returns the variable R
    std::vector< std::vector<double> > get_R();

    //! Returns the variable H
    GF2Mat get_H();

    //! Calculates the likelihood ratios given an input vector
    void rx_lr_calc(std::vector<float> codeword);

    //! Returns the variable rx_lr
    std::vector<double> get_rx_lr();

    //! Returns the variable lr
    std::vector<double> get_lr();

    //! Initializes the sum product algorithm set-up
    void spa_initialize();

    //! Updates the check-nodes based on messages from variable nodes
    void update_chks();

    //! Updates the variable-nodes based on messages from check nodes
    void update_vars();

    //! Returns the current estimate
    std::vector<char> get_estimate();

    //! Computes initial estimate based on the vector rx_word
    void compute_init_estimate(std::vector<float> rx_word);

    //! Computes the estimate based on current likelihood-ratios lr
    void decision();

    //! Returns the syndrome for the current estimate
    std::vector<char> get_syndrome();

    //! Returns the syndrome for the input codeword
    std::vector<char> get_syndrome(const std::vector<char> codeword);

    //! Checks if the current estimate is a codeword
    bool is_codeword();

    //! Checks if the input is a codeword
    bool is_codeword(const std::vector<char> codeword);

    //! Sets the variable K
    void set_K(int k);

    //! Returns the variable K
    int get_K();

    //! Sets the variable max_iterations
    void set_max_iterations(int k);

    //! Returns the variable max_iterations
    int get_max_iterations();

    /*!
     * \brief Decodes the given vector rx_word by message passing.
     *
     * \param rx_word The received samples for decoding.
     * \param niterations The number of message passing iterations
     *        done to decode this codeword.
    */
    std::vector<char> decode (std::vector<float> rx_word,
                              int *niterations);
  private:
    //! The number of check nodes in the tanner-graph
    int M;

    //! The number of variable nodes in the tanner-graph
    int N;

    //! The dimension of the code used
    int K;

    //! The maximum number of message passing iterations allowed
    int max_iterations;

    //! The parity check matrix of the LDPC code
    GF2Mat H;

    //! The standard-deviation of the AWGN channel
    float sigma;

    //! Matrix holding messages from check nodes to variable nodes
    std::vector< std::vector<double> > R;

    //! Matrix holding messages from variable nodes to check nodes
    std::vector< std::vector<double> > Q;

    //! The array of likelihood computed from the channel output
    std::vector<double> rx_lr;

    //! The array for holding likelihoods computed on BP decoding
    std::vector<double> lr;

    //! List of integer coordinates along each column with non-zero entries
    std::vector < std::vector<int> > nlist;

    //! List of integer coordinates along each row with non-zero entries
    std::vector < std::vector<int> > mlist;

    //! Weight of each column n
    std::vector <int> num_nlist;

    //! Weight of each row m
    std::vector <int> num_mlist;

    //! The array for holding estimate computed on BP decoding
    std::vector<char> estimate;
};
#endif // ifndef AWGN_BP_H
