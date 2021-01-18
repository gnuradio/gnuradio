/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* -----------------------------------------------------------------
 *
 * This class handles sparse matrices specified in alist-format.
 * For details about alist format please visit the link below.
 * - http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
 *
 * Alist class is an efficient way of representing a sparse matrix
 * the parity check matrix H of an LDPC code for instance.
 *
 */

#ifndef ALIST_H
#define ALIST_H

#include <gnuradio/fec/api.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

class FEC_API alist
{
public:
    //! Default Constructor
    alist() : data_ok(false) {}

    //! Constructor which loads alist class from an alist-file
    alist(const char* fname);

    //! Read alist data from a file
    void read(const char* fname);

    //! Write alist data to a file
    void write(const char* fname) const;

    //! Returns N, the number of variable nodes
    int get_N();

    //! Return M, the number of check nodes
    int get_M();

    //! Return the m_list variable
    std::vector<std::vector<int>> get_mlist();

    //! Returns the n_list variable
    std::vector<std::vector<int>> get_nlist();

    //! Returns the num_mlist variable
    std::vector<int> get_num_mlist();

    //! Returns the num_nlist variable
    std::vector<int> get_num_nlist();

    //! Returns the max_num_nlist variable
    int get_max_num_nlist();

    //! Returns the max_num_mlist variable
    int get_max_num_mlist();

    //! Prints the nlist[i] variable
    void print_nlist_i(int i);

    //! Prints the mlist[i] variable
    void print_mlist_i(int i);

    //! Returns the corresponding H matrix
    std::vector<std::vector<uint8_t>> get_matrix();

protected:
    //! A variable indicating if data has been read from alist-file
    bool data_ok;

    //! Number of variable nodes
    int N;

    //! Number of check nodes
    int M;

    //! Maximum weight of rows
    int max_num_mlist;

    //! Maximum weight of columns
    int max_num_nlist;

    //! Weight of each column n
    std::vector<int> num_nlist;

    //! Weight of each row m
    std::vector<int> num_mlist;

    //! List of integer coordinates along each rows with non-zero entries
    std::vector<std::vector<int>> mlist;

    //! List of integer coordinates along each column with non-zero entries
    std::vector<std::vector<int>> nlist;
};
#endif // ifndef ALIST_H
