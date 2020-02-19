/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef LDPC_H
#define LDPC_H

#include <iostream>
#include <vector>

#include "gnuradio/fec/alist.h"
#include "gnuradio/fec/gf2mat.h"
#include "gnuradio/fec/gf2vec.h"


#include <gnuradio/fec/api.h>
class FEC_API cldpc
{
public:
    //! Default constructor
    cldpc(){};

    //! Constructs the LDPC class from given GF2mat X
    cldpc(const GF2Mat X);

    //! Constructs the class from the given alist _list
    cldpc(const alist _list);

    //! Prints the variable permute
    void print_permute();

    /*!
      \brief Encode the given vector dataword.

      dataword is of length K where K is the dimension of the code.
      The function returns a vector of length N where N is the
      block-length of the code.

      For encoding a G matrix in the form [I P] is obtained from the
      parity matrix H, by (a) Column permutations, (b) Row additions
      and (c) Row permutations. Details of encoding is given in
      section A.1 of the reference given below.
       - "Modern Coding Theory", T Richardson and R Urbanke.
    */
    std::vector<char> encode(std::vector<char> dataword);

    //! Returns the dimension of the code
    int dimension();

    //! Returns the parity check matrix H
    GF2Mat get_H();

    //! Returns the matrix G used in encoding
    GF2Mat get_G();

    //! Returns the variable M
    int get_M();

    //! Returns the variable N
    int get_N();

    //! Returns the syndrome for a given vector "in"
    std::vector<char> syndrome(const std::vector<char> in);

    //! Returns true if "in" is a codeword, else false
    bool is_codeword(const std::vector<char> in);

    //! Set the variable _list
    void set_alist(const alist _list);

    //! Obtain systematic bits from "in"
    std::vector<char> get_systematic_bits(std::vector<char> in);

private:
    //! The parity check matrix
    GF2Mat H;

    //! An equivalent matrix obtained from H used for encoding
    GF2Mat G;

    //! Stores the column permutation in obtaining G from H
    std::vector<int> permute;

    //! Rank of the H matrix
    int rank_H;

    //! The number of check nodes in the Tanner-graph
    int M;

    //! The number of variable nodes in the Tanner-graph
    int N;

    //! The dimension of the code
    size_t K;
};

#endif // ifndef LDPC_H
