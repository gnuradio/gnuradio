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

#include "ldpc_G_matrix_impl.h"
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

namespace gr {
  namespace fec {
    namespace code {

      ldpc_G_matrix::sptr
      ldpc_G_matrix::make(const std::string filename)
      {
        return ldpc_G_matrix::sptr
          (new ldpc_G_matrix_impl(filename));
      }

      ldpc_G_matrix_impl::ldpc_G_matrix_impl(const std::string filename)
        : fec_mtrx_impl()
      {
        configure_default_loggers(d_logger, d_debug_logger, "ldpc_G_matrix");

        // Read the matrix from a file in alist format
        matrix_sptr x = read_matrix_from_file(filename);
        d_num_cols = x->size2;
        d_num_rows = x->size1;

        // Make an actual copy so we guarantee that we're not sharing
        // memory with another class that reads the same alist file.
        gsl_matrix *G = gsl_matrix_alloc(d_num_rows, d_num_cols);
        gsl_matrix_memcpy(G, (gsl_matrix*)(x.get()));

        unsigned int row_index, col_index;

        // First, check if we have a generator matrix G in systematic
        // form, G = [I P], where I is a k x k identity matrix and P
        // is the parity submatrix.

        // Length of codeword = # of columns of generator matrix
        d_n = d_num_cols;
        // Length of information word = # of rows of generator matrix
        d_k = d_num_rows;

        gsl_matrix *I_test   = gsl_matrix_alloc(d_k, d_k);
        gsl_matrix *identity = gsl_matrix_alloc(d_k, d_k);
        gsl_matrix_set_identity(identity);

        for(row_index = 0; row_index < d_k; row_index++) {
          for(col_index = 0; col_index < d_k; col_index++) {
            int value = gsl_matrix_get(G, row_index, col_index);
            gsl_matrix_set(I_test, row_index, col_index, value);
          }
        }

        // Check if the identity matrix exists in the right spot.
        //int test_if_equal = gsl_matrix_equal(identity, I_test);
        gsl_matrix_sub(identity, I_test); // should be null set if equal
        double test_if_not_equal = gsl_matrix_max(identity);

        // Free memory
        gsl_matrix_free(identity);
        gsl_matrix_free(I_test);

        //if(!test_if_equal) {
        if(test_if_not_equal > 0) {
          GR_LOG_ERROR(d_logger,
                       "Error in ldpc_G_matrix_impl constructor. It appears "
                       "that the given alist file did not contain either a "
                       "valid parity check matrix of the form H = [P' I] or "
                       "a generator matrix of the form G = [I P].\n");
          throw std::runtime_error("ldpc_G_matrix: Bad matrix definition");
        }

        // Our G matrix is verified as correct, now convert it to the
        // parity check matrix.

        // Grab P matrix
        gsl_matrix *P = gsl_matrix_alloc(d_k, d_n-d_k);
        for(row_index = 0; row_index < d_k; row_index++) {
          for(col_index = 0; col_index < d_n-d_k; col_index++) {
            int value = gsl_matrix_get(G, row_index, col_index + d_k);
            gsl_matrix_set(P, row_index, col_index, value);
          }
        }

        // Calculate P transpose
        gsl_matrix *P_transpose = gsl_matrix_alloc(d_n-d_k, d_k);
        gsl_matrix_transpose_memcpy(P_transpose, P);

        // Set H matrix. H = [-P' I] but since we are doing mod 2,
        // -P = P, so H = [P' I]
        gsl_matrix *H_ptr = gsl_matrix_alloc(d_n-d_k, d_n);
        gsl_matrix_set_zero(H_ptr);
        for(row_index = 0; row_index < d_n-d_k; row_index++) {
          for(col_index = 0; col_index < d_k; col_index++) {
            int value = gsl_matrix_get(P_transpose, row_index, col_index);
            gsl_matrix_set(H_ptr, row_index, col_index, value);
          }
        }

        for(row_index = 0; row_index < (d_n-d_k); row_index++) {
          col_index = row_index + d_k;
          gsl_matrix_set(H_ptr, row_index, col_index, 1);
        }

        // Calculate G transpose (used for encoding)
        d_G_transp_ptr = gsl_matrix_alloc(d_n, d_k);
        gsl_matrix_transpose_memcpy(d_G_transp_ptr, G);

        d_H_sptr = matrix_sptr((matrix*)H_ptr);

        // Free memory
        gsl_matrix_free(P);
        gsl_matrix_free(P_transpose);
        gsl_matrix_free(G);
      }


      const gsl_matrix*
      ldpc_G_matrix_impl::G_transpose() const
      {
        const gsl_matrix *G_trans_ptr = d_G_transp_ptr;
        return G_trans_ptr;
      }

      void
      ldpc_G_matrix_impl::encode(unsigned char *outbuffer,
                                  const unsigned char *inbuffer) const
      {

        unsigned int index, k = d_k, n = d_n;
        gsl_matrix *s = gsl_matrix_alloc(k, 1);
        for(index = 0; index < k; index++) {
          double value = static_cast<double>(inbuffer[index]);
          gsl_matrix_set(s, index, 0, value);
        }

        // Simple matrix multiplication to get codeword
        gsl_matrix *codeword = gsl_matrix_alloc(G_transpose()->size1, s->size2);
        mult_matrices_mod2(codeword, G_transpose(), s);

        // Output
        for(index = 0; index < n; index++) {
          outbuffer[index] = gsl_matrix_get(codeword, index, 0);
        }

        // Free memory
        gsl_matrix_free(s);
        gsl_matrix_free(codeword);
      }


      void
      ldpc_G_matrix_impl::decode(unsigned char *outbuffer,
                                 const float *inbuffer,
                                 unsigned int frame_size,
                                 unsigned int max_iterations) const
      {
        unsigned int index, n = d_n;
        gsl_matrix *x = gsl_matrix_alloc(n, 1);
        for (index = 0; index < n; index++) {
          double value = inbuffer[index] > 0 ? 1.0 : 0.0;
          gsl_matrix_set(x, index, 0, value);
        }

        // Initialize counter
        unsigned int count = 0;

        // Calculate syndrome
        gsl_matrix *syndrome = gsl_matrix_alloc(H()->size1, x->size2);
        mult_matrices_mod2(syndrome, H(), x);

        // Flag for finding a valid codeword
        bool found_word = false;

        // If the syndrome is all 0s, then codeword is valid and we
        // don't need to loop; we're done.
        if (gsl_matrix_isnull(syndrome)) {
          found_word = true;
        }

        // Loop until valid codeword is found, or max number of
        // iterations is reached, whichever comes first
        while ((count < max_iterations) && !found_word) {
          // For each of the n bits in the codeword, determine how
          // many of the unsatisfied parity checks involve that bit.
          // To do this, first find the nonzero entries in the
          // syndrome. The entry numbers correspond to the rows of
          // interest in H.
          std::vector<int> rows_of_interest_in_H;
          for (index = 0; index < (*syndrome).size1; index++) {
            if (gsl_matrix_get(syndrome, index, 0)) {
              rows_of_interest_in_H.push_back(index);
            }
          }

          // Second, for each bit, determine how many of the
          // unsatisfied parity checks involve this bit and store
          // the count.
          unsigned int i, col_num, n = d_n;
          std::vector<int> counts(n,0);
          for (i = 0; i < rows_of_interest_in_H.size(); i++) {
            unsigned int row_num = rows_of_interest_in_H[i];
            for (col_num = 0; col_num < n; col_num++) {
              double value = gsl_matrix_get(H(),
                                            row_num,
                                            col_num);
              if (value > 0) {
                counts[col_num] = counts[col_num] + 1;
              }
            }
          }

          // Next, determine which bit(s) is associated with the most
          // unsatisfied parity checks, and flip it/them.
          int max = 0;
          for (index = 0; index < n; index++) {
            if (counts[index] > max) {
              max = counts[index];
            }
          }

          for (index = 0; index < n; index++) {
            if (counts[index] == max) {
              unsigned int value = gsl_matrix_get(x, index, 0);
              unsigned int new_value = value ^ 1;
              gsl_matrix_set(x, index, 0, new_value);
            }
          }

          // Check the syndrome; see if valid codeword has been found
          mult_matrices_mod2(syndrome, H(), x);
          if (gsl_matrix_isnull(syndrome)) {
            found_word = true;
            break;
          }
          count++;
        }

        // Extract the info word and assign to output. This will
        // happen regardless of if a valid codeword was found.
        if(parity_bits_come_last()) {
          for(index = 0; index < frame_size; index++) {
            outbuffer[index] = gsl_matrix_get(x, index, 0);
          }
        }
        else {
          for(index = 0; index < frame_size; index++) {
            unsigned int i = index + n - frame_size;
            int value = gsl_matrix_get(x, i, 0);
            outbuffer[index] = value;
          }
        }

        // Free memory
        gsl_matrix_free(syndrome);
        gsl_matrix_free(x);
      }

      gr::fec::code::fec_mtrx_sptr
      ldpc_G_matrix_impl::get_base_sptr()
      {
        return shared_from_this();
      }

      ldpc_G_matrix_impl::~ldpc_G_matrix_impl()
      {
        // Call the gsl_matrix_free function to free memory.
        gsl_matrix_free(d_G_transp_ptr);
        gsl_matrix_free(d_H_obj);
      }
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
