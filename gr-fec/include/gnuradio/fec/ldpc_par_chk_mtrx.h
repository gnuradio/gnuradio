/* -*- c++ -*- */
/* 
 * Copyright 2013-2014 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation; either version 3, or (at your 
 * option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_ldpc_par_chk_mtrx_H
#define INCLUDED_ldpc_par_chk_mtrx_H

#include <gnuradio/fec/api.h>
#include <string>
 
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

namespace gr {
  namespace fec {
    namespace code {
      class ldpc_par_chk_mtrx
      {
        // Codeword length n (also the number of columns in the H
        // matrix)
        unsigned int d_n;
        // Information word length k
        unsigned int d_k;
        // Rank of the parity check matrix
        unsigned int d_rank;
        // Number of rows in the parity check matrix
        unsigned int d_num_rows;
        // Gap (when matrix is in TABECD form)
        unsigned int d_gap;
        // GSL matrix structure for the parity check matrix
        gsl_matrix *d_H_ptr;

      public:
        /* Overloading the constructor because the parity
           matrix may come from one of three places and be in a
           few forms:
            1. Be read in from a given filename (must be alist
               format)
               a) encoding-ready. (Gap g must be provided)
                  Set flag = 1. This is the default flag/case.
               b) full rank but not encoding ready
                  Set flag = 2.
               c) regular LDPC matrix, not (necessarily) full rank
                  Set flag = 3.
               d) unknown state, so try (c)
                  Set flag = 4.
            2. Be already existing in a GSL matrix object. User
               must call functions as needed to get matrix into
               encoding-ready state
            3. Be generated by this class if the properties are
               specified
        */

        // Constructor if filename is given, case 1
        ldpc_par_chk_mtrx(std::string *, 
                          int flag = 1,
                          unsigned int gap = 0);

        // Constructor if a parity check matrix is given, case 2
        ldpc_par_chk_mtrx(gsl_matrix *);

        // Constructor if n,p,q values are given, case 3
        ldpc_par_chk_mtrx(unsigned int n, 
                          unsigned int p,
                          unsigned int q);

        // Default constructor, should not be used
        ldpc_par_chk_mtrx();

        // Get the codeword length n
        unsigned int get_n();
        // Get the information word length k
        unsigned int get_k();
        // Get the rank of the parity check matrix
        unsigned int get_rank();

        // These are the submatrices found during preprocessing
        // which are used for encoding. 
        // TODO should these be private?
        gsl_matrix *d_T_inverse;
        gsl_matrix *d_phi_inverse;
        gsl_matrix *d_E;
        gsl_matrix *d_A;
        gsl_matrix *d_B;
        gsl_matrix *d_D;

        // Write the matrix out to a file in alist format
        void write_matrix_to_file(std::string *);
        // Read the matrix from a file in alist format
        void read_matrix_from_file(std::string *);
        // Reduce a H matrix which is not full rank to be full rank
        void reduce_H_to_full_rank_matrix();
        // Transform full rank parity check H matrix to TABECD form
        void full_rank_to_TABECD_form(bool extra_shuffle=false);
        // Preprocess TABECD form matrix to get parameters for
        // encoding. Should only do this one time because it takes 
        // a while...
        void preprocess_for_encoding();
        // Set the submatrix variables needed for encoding
        void set_parameters_for_encoding();
        // Subtract matrices using mod2 operation
        gsl_matrix *subtract_matrices_mod2(gsl_matrix *,
                                           gsl_matrix *);
        // Perform matrix multiplication using mod 2 operations
        gsl_matrix *matrix_mult_mod2(gsl_matrix *, gsl_matrix *);
        // Find the inverse of a square matrix using modulo 2
        // operations
        gsl_matrix *calc_inverse_mod2(gsl_matrix *);

        // Destructor
        ~ldpc_par_chk_mtrx();
      };
    }
  }
}

 #endif /* INCLUDED_ldpc_par_chk_mtrx_H */