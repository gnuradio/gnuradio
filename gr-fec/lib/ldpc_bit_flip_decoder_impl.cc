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

#include <ldpc_bit_flip_decoder_impl.h>
#include <math.h>
#include <boost/assign/list_of.hpp>
#include <volk/volk.h>
#include <sstream>
#include <stdio.h>
#include <vector>

namespace gr {
  namespace fec {
    namespace code {

      generic_decoder::sptr
      ldpc_bit_flip_decoder::make(ldpc_par_chk_mtrx *H_obj,
                                  unsigned int max_iter)
      {
        return generic_decoder::sptr
          (new ldpc_bit_flip_decoder_impl(H_obj, max_iter));
      }

      ldpc_bit_flip_decoder_impl::ldpc_bit_flip_decoder_impl(ldpc_par_chk_mtrx *H_obj, unsigned int max_iter)
        : generic_decoder("ldpc_bit_flip_decoder")
      {
        // LDPC parity check matrix to use for decoding
        d_H = H_obj;
        // Set frame size to k, the # of bits in the information word
        // All buffers and settings will be based on this value.
        set_frame_size(d_H->k());
        // Maximum number of iterations in the decoding algorithm
        d_max_iterations = max_iter;
      }

      ldpc_bit_flip_decoder_impl::~ldpc_bit_flip_decoder_impl()
      {
      }

      int
      ldpc_bit_flip_decoder_impl::get_output_size()
      {
        return d_frame_size;
      }
      
      int
      ldpc_bit_flip_decoder_impl::get_input_size()
      {
        return int(d_H->n());
      }

      bool
      ldpc_bit_flip_decoder_impl::set_frame_size(
                                            unsigned int frame_size)
      {
        bool ret = true;
        // TODO add some bounds check here? The frame size is
        // constant and specified by the size of the parity check
        // matrix used for encoding.
        d_frame_size = frame_size;

        return ret;
      }

      double
      ldpc_bit_flip_decoder_impl::rate()
      {
        return static_cast<double>(d_frame_size)/(d_H->n());
      }


      void
      ldpc_bit_flip_decoder_impl::generic_work(void *inbuffer,
                                               void *outbuffer)
      {

        // Populate the information word
        const float *in = (const float*)inbuffer;

        unsigned int index, n = d_H->n();
        gsl_matrix *x = gsl_matrix_alloc(n, 1);
        for (index = 0; index < n; index++) {
          double value = static_cast<double>(in[index]);
          if (value == -1) {
            value = 0;
          }
          gsl_matrix_set(x, index, 0, value);
        }

        // Parity check matrix to use
        const gsl_matrix *H = d_H->H();

        // Initialize counter
        unsigned int count = 0;

        // Calculate syndrome
        gsl_matrix *syndrome = d_H->mult_matrices_mod2(H, x);

        // Flag for finding a valid codeword
        bool found_word = false;
 
        // If the syndrome is all 0s, then codeword is valid and we
        // don't need to loop; we're done.
        if (gsl_matrix_isnull(syndrome)) {
          found_word = true;
        }

        // Loop until valid codeword is found, or max number of
        // iterations is reached, whichever comes first
        while ((count < d_max_iterations) && !found_word) {
          // For each of the n bits in the codeword, determine how
          // many of the unsatisfied parity checks involve that bit.
          // To do this, first find the nonzero entries in the
          // syndrome. The entry numbers correspond to the rows of
          // interest in H.
          std::vector<int> rows_of_interest_in_H;
          for (index = 0; index < (*syndrome).size1; index++) {;
            if (gsl_matrix_get(x, index, 0)) {
              rows_of_interest_in_H.push_back(index);
            }
          }

          // Second, for each bit, determine how many of the
          // unsatisfied parity checks involve this bit and store
          // the count.
          unsigned int i, col_num, n = d_H->n();
          std::vector<int> counts(n,0);
          for (i = 0; i < rows_of_interest_in_H.size(); i++) {
            unsigned int row_num = rows_of_interest_in_H[i];
            for (col_num = 0; col_num < n; col_num++) {
              double value = gsl_matrix_get(H, row_num, col_num);
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
          syndrome = d_H->mult_matrices_mod2(H, x);
          if (gsl_matrix_isnull(syndrome)) {
            found_word = true;
            break;
          }
          count++;
        }

        // After finding the valid codeword, extract the info word
        // and assign to output.
        unsigned char *out = (unsigned char*) outbuffer;
        for (index = 0; index < d_frame_size; index++) {
          unsigned int i = index + n - d_frame_size;
          int value = gsl_matrix_get(x, i, 0);
          out[index] = value;
        }

        if (!found_word) {
          throw std::runtime_error("ldpc_bit_flip_decoder: valid codeword not found.");
        }
      } /* ldpc_bit_flip_decoder_impl::generic_work() */     
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

