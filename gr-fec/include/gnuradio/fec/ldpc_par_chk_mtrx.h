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
      class FEC_API ldpc_par_chk_mtrx
      {
        // Codeword length n (also the number of columns in the H
        // matrix)
        unsigned int d_n;
        // Information word length k
        unsigned int d_k;
        // Number of rows in the parity check matrix
        unsigned int d_num_rows;
        // Gap (assumes matrix is in TABECD form)
        unsigned int d_gap;
        // GSL matrix structure for the parity check matrix
        gsl_matrix *d_H_ptr;
        // These are the submatrices found during preprocessing
        // which are used for encoding. 
        gsl_matrix *d_A_ptr;
        gsl_matrix *d_B_ptr;
        gsl_matrix *d_D_ptr;
        gsl_matrix *d_E_ptr;
        gsl_matrix *d_T_inverse_ptr;
        gsl_matrix *d_phi_inverse_ptr;

      public:
        ldpc_par_chk_mtrx(std::string *, unsigned int gap);

        // Default constructor, should not be used
        ldpc_par_chk_mtrx();

        // Get the codeword length n
        unsigned int n();
        // Get the information word length k
        unsigned int k();
        // Access the matrices needed during encoding
        const gsl_matrix *H();
        const gsl_matrix *A();
        const gsl_matrix *B();
        const gsl_matrix *D();
        const gsl_matrix *E();
        const gsl_matrix *T_inverse();
        const gsl_matrix *phi_inverse();
        // Read the matrix from a file in alist format
        void read_matrix_from_file(std::string *);
        // Set the submatrix variables needed for encoding
        void set_parameters_for_encoding();
        // Subtract matrices using mod2 operation
        gsl_matrix *subtract_matrices_mod2(gsl_matrix *,
                                           gsl_matrix *);
        // Perform matrix multiplication using mod 2 operations
        gsl_matrix *mult_matrices_mod2(gsl_matrix *, gsl_matrix *);
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