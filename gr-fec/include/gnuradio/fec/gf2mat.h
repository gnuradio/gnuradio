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

#ifndef GF2MAT_H
#define GF2MAT_H
#include <vector>
#include "gf2vec.h"
#include "alist.h"

class GF2Mat
{
    //! The matrix H
    std::vector < std::vector <char> > H;

    //! Number of rows in H
    int M;

    //! Number of columns in H
    int N;

  public:
    //! Default constructor
    GF2Mat() {};

    //! Construct an M x N matrix with all 0 entries
    GF2Mat(int m, int n);

    //! Loads the matrix from alist _list
    GF2Mat(alist _list);

    //! Initializes the class from a 2-D vector X
    GF2Mat(std::vector <std::vector <char> > X);

    //! Returns the variable M
    int get_M();

    //! Returns the variable N
    int get_N();

    //! Set the element at (i, j) coordinate to val
    void set_element(int i, int j, char val);

    //! Returns the element at coordinate (i, j)
    char get_element(int i, int j);

    //! Returns the ith row
    GF2Vec get_row(int i);

    //! Returns the ith column
    GF2Vec get_col(int i);

    //! Returns the ith row
    GF2Vec operator[](int i);

    //! Prints the matrix H
    void print_matrix();

    //! Sets the ith column with the given vector
    void set_col(int i, GF2Vec vec);

    //! Sets the ith row with the given vector
    void set_row(int i, GF2Vec vec);

    //! Swaps columns i and j
    void swap_cols(int i, int j);

    //! Adds column j to i and replace i with the sum
    void add_cols(int i, int j);

    //! Add row j to i and replace j with the sum
    void add_rows(int i, int j);

    //! Returns the variable H
    std::vector<std::vector<char> > get_H();

    /*!
     * \brief Obtains an equivalent representation of H for encoding
     *
     * For encoding a G matrix in the form [I P] obtained from the
     * parity matrix H, by (a) Column permutations, (b) Row additions
     * and (c) Row permutations. Details of encoding is given in
     * section A.1 of the reference:
     *
     *  - "Modern Coding Theory", T Richardson and R Urbanke.
     *
     * \param p The column permutation during this operation.
     * \param rank The rank of the matrix.
     */
    GF2Mat get_G(std::vector<int> & p, int & rank);

};

#endif // #ifndef GF2MAT_H
