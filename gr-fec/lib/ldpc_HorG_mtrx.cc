/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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

#include <gnuradio/fec/ldpc_HorG_mtrx.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

namespace gr {
  namespace fec {
    namespace code {

      ldpc_HorG_mtrx::ldpc_HorG_mtrx(const std::string filename) 
      {
        // Read the matrix from a file in alist format
        gsl_matrix *temp_mtrx = read_matrix_from_file(filename);

        // Next, figure out if the format is [I P] or [P' I]
        bool have_G_matrix = false;
        bool have_H_matrix = false;

        unsigned int row_index, col_index;
        // First, check if we have a generator matrix G in systematic
        // form, G = [I P], where I is a k x k identity matrix and P
        // is the parity submatrix.
        try {
          // Length of codeword = # of columns of generator matrix
          d_n = d_num_cols;
          // Length of information word = # of rows of generator matrix
          d_k = d_num_rows;

          gsl_matrix *I_test   = gsl_matrix_alloc(d_k,d_k);
          gsl_matrix *identity = gsl_matrix_alloc(d_k,d_k);
          gsl_matrix_set_identity(identity);

          for (row_index = 0; row_index < d_k; row_index++) {
            for (col_index = 0; col_index < d_k; col_index++) {
              int value = gsl_matrix_get(temp_mtrx,
                                         row_index,
                                         col_index);
              gsl_matrix_set(I_test, row_index, col_index, value);
            }
          }

          int test_if_equal = gsl_matrix_equal(identity,I_test);
          if (test_if_equal) {
            have_G_matrix = true;
          }

          // Free memory
          gsl_matrix_free(identity);
          gsl_matrix_free(I_test);
        }
        catch (...) {
          // Must not have been a G matrix.
          // No need to output anything.
        }

        // Second, check if we have a parity check matrix H in the standard form, H = [P' I]
        try {
          // Length of codeword = # of columns of generator matrix
          d_n = d_num_cols;
          // Length of information word
          d_k = d_n - d_num_rows;

          gsl_matrix *I_test   = gsl_matrix_alloc(d_n-d_k, d_n-d_k);
          gsl_matrix *identity = gsl_matrix_alloc(d_n-d_k, d_n-d_k);
          gsl_matrix_set_identity(identity);

          for (row_index = 0; row_index < d_n-d_k; row_index++) {
            for (col_index = d_k; col_index < d_n; col_index++) {
              int value = gsl_matrix_get(temp_mtrx,
                                         row_index,
                                         col_index);
              gsl_matrix_set(I_test,
                             row_index,
                             col_index - d_k,
                             value);
            }
          }

          int test_if_equal = gsl_matrix_equal(identity,I_test);
          if (test_if_equal) {
            have_H_matrix = true;
          }

          // Free memory
          gsl_matrix_free(identity);
          gsl_matrix_free(I_test);
        }
        catch (...) {
          // Must not have been an H matrix.
          // No need to output anything.
        }

        // If neither, exit
        if (!have_G_matrix && !have_H_matrix) {
          std::cout << " ***Error in ldpc_HorG_mtrx constructor. It "
                    << "appears that the given alist file did not "
                    << "contain either a valid parity check matrix "
                    << "of the form H = [P' I] or a generator matrix"
                    << " of the form G = [I P].\n";
          exit(1);
        }

        if (have_G_matrix) {
          gsl_matrix *d_G_ptr = temp_mtrx;

          // Grab P matrix
          gsl_matrix *P = gsl_matrix_alloc(d_k,d_n-d_k);
          for (row_index = 0; row_index < d_k; row_index++) {
            for (col_index = 0; col_index < d_n-d_k; col_index++) {
              int value = gsl_matrix_get(d_G_ptr,
                                         row_index,
                                         col_index + d_k);
              gsl_matrix_set(P, row_index, col_index, value);
            }
          }

          // Calculate P transpose
          gsl_matrix *P_transpose = gsl_matrix_alloc(d_n-d_k, d_k);
          gsl_matrix_transpose_memcpy(P_transpose, P);

          // Set H matrix. H = [-P' I] but since we are doing mod 2,
          // -P = P, so H = [P' I]
          d_H_ptr = gsl_matrix_alloc(d_n-d_k, d_n);
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

          // Calculate G transpose (used for encoding)
          d_G_transp_ptr = gsl_matrix_alloc(d_n, d_k);
          gsl_matrix_transpose_memcpy(d_G_transp_ptr, d_G_ptr);

          // Free memory
          gsl_matrix_free(P);
          gsl_matrix_free(d_G_ptr);
          gsl_matrix_free(P_transpose);
        }

        if (have_H_matrix) {
          // Set H matrix (used for decoding)
          d_H_ptr = temp_mtrx;

          // Grab P' matrix (P' denotes P transposed)
          gsl_matrix *P_transpose = gsl_matrix_alloc(d_n-d_k,d_k);
          for (row_index = 0; row_index < d_n-d_k; row_index++) {
            for (col_index = 0; col_index < d_k; col_index++) {
              int value = gsl_matrix_get(d_H_ptr,
                                         row_index,
                                         col_index);
              gsl_matrix_set(P_transpose, row_index, col_index, value);
            }
          }

          // Set G transpose matrix (used for encoding)
          d_G_transp_ptr = gsl_matrix_alloc(d_n, d_k);
          gsl_matrix_set_zero(d_G_transp_ptr);
          for (row_index = 0; row_index < d_k; row_index++) {
            col_index = row_index;
            gsl_matrix_set(d_G_transp_ptr, row_index, col_index, 1);
          }
          for (row_index = d_k; row_index < d_n; row_index++) {
            for (col_index = 0; col_index < d_k; col_index++) {
              int value = gsl_matrix_get(P_transpose,
                                         row_index - d_k,
                                         col_index);
              gsl_matrix_set(d_G_transp_ptr,
                             row_index,
                             col_index,
                             value);
            }
          }

          // Free memory
          gsl_matrix_free(P_transpose);
        }

        // For info about this see get_base_ptr() function
        d_base_ptr = this;

      } // Constructor

      // Default constructor, should not be used
      ldpc_HorG_mtrx::ldpc_HorG_mtrx()
      {
        std::cout << "Error in ldpc_HorG_mtrx(): Default "
                  << "constructor called.\nMust provide filename for"
                  << " generator matrix. \n\n";
        exit(1);
      } // Default constructor

      const gsl_matrix*
      ldpc_HorG_mtrx::G_transpose() const
      {
        const gsl_matrix *G_trans_ptr = d_G_transp_ptr;
        return G_trans_ptr;
      }

      gr::fec::code::fec_mtrx*
      ldpc_HorG_mtrx::get_base_ptr()
      {
        /* For some reason, swig isn't accepting a pointer to this
           child class for the make function of the
           ldpc_bit_flip_decoder; it's expecting a pointer to the
           base class. So this is just a workaround for swig/GRC.
        */
        return d_base_ptr;
      }

      ldpc_HorG_mtrx::~ldpc_HorG_mtrx()
      {
        // Call the gsl_matrix_free function to free memory.
        gsl_matrix_free(d_H_ptr);
        gsl_matrix_free(d_G_transp_ptr);
      }
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */