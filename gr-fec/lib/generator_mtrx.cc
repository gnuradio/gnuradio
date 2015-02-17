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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/fec/generator_mtrx.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

namespace gr {
  namespace fec {
    namespace code {

      generator_mtrx::generator_mtrx(const std::string filename) 
      {
        // Read the matrix from a file in alist format
        read_matrix_from_file(filename, d_G_ptr);

        // The alist file should have provided a generator matrix G
        // in systematic form, G = [I P], where I is a k x k identity
        // matrix and P is the parity submatrix. So start by checking
        // that the left side k x k submatrix is I.

        gsl_matrix *I_test   = gsl_matrix_alloc(d_k,d_k);
        gsl_matrix *identity = gsl_matrix_alloc(d_k,d_k);
        gsl_matrix_set_identity(identity);

        unsigned int row_index, col_index;
        for (row_index = 0; row_index < d_k; row_index++) {
          for (col_index = 0; col_index < d_k; col_index++) {
            int value = gsl_matrix_get(d_G_ptr, row_index,col_index);
            gsl_matrix_set(I_test, row_index, col_index, value);
          }
        }

        int test_if_equal = gsl_matrix_equal(identity,I_test);
        if (!test_if_equal) {
          throw "Error in generator_mtrx constructor. It appears that the given alist file did not contain a valid generator matrix of the form G = [I P].\n";
        }

        // Length of codeword = # of columns of generator matrix
        d_n = d_num_cols;
        // Length of information word = # of rows of generator matrix
        d_k = d_num_rows;

        // Grab P matrix
        gsl_matrix *P = gsl_matrix_alloc(d_k,d_n-d_k);
        for (row_index = 0; row_index < d_k; row_index++) {
          for (col_index = 0; col_index < d_n-d_k; col_index++) {
            int value = gsl_matrix_get(d_G_ptr, row_index,col_index);
            gsl_matrix_set(P, row_index, col_index, value);
          }
        }

        // Calculate P transpose
        gsl_matrix *P_transpose = gsl_matrix_alloc(d_n-d_k, d_k);
        gsl_matrix_transpose_memcpy(P_transpose, P);

        // Set H matrix. H = [-P' I] but since we are doing mod 2,
        // -P = P, so H = [P' I]
        d_H_ptr = gsl_matrix_alloc(d_n-d_k, d_k);
        gsl_matrix_set_zero(d_H_ptr);
        for (row_index = 0; row_index < d_k; row_index++) {
          for (col_index = 0; col_index < d_n-d_k; col_index++) {
            int value = gsl_matrix_get(P_transpose,
                                       row_index,
                                       col_index);
            gsl_matrix_set(d_H_ptr, row_index, col_index, value);
          }
        }
        for (row_index = 0; row_index < d_k; row_index++) {
          col_index = row_index + d_k;
          gsl_matrix_set(d_H_ptr, row_index, col_index, 1);
        }

        // Free memory
        gsl_matrix_free(P);
        gsl_matrix_free(P_transpose);
        gsl_matrix_free(identity);
        gsl_matrix_free(I_test);

      } // Constructor

      // Default constructor, should not be used
      generator_mtrx::generator_mtrx()
      {
        std::cout << "Error in generator_mtrx(): Default "
                  << "constructor called.\nMust provide filename for"
                  << " generator matrix. \n\n";
        exit(1);
      } // Default constructor

      unsigned int
      generator_mtrx::n()
      {
        return d_n;
      }

      unsigned int
      generator_mtrx::k()
      {
        return d_k;
      }
      
      const gsl_matrix*
      generator_mtrx::G()
      {
        const gsl_matrix *G_ptr = d_G_ptr;
        return G_ptr; 
      }

      generator_mtrx::~generator_mtrx()
      {
        // Call the gsl_matrix_free function to free memory.
        gsl_matrix_free(d_H_ptr);
        gsl_matrix_free(d_G_ptr);
      }
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */