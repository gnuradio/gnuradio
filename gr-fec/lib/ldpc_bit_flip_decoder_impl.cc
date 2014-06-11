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
      ldpc_bit_flip_decoder::make(
                        LDPC_par_chk_mtrx_impl parity_check_matrix,
                        unsigned int max_iterations,
                        unsigned int frame_size,
                        unsigned int n);
      {
        return generic_decoder::sptr
          (new ldpc_bit_flip_decoder_impl(parity_check_matrix,
                                          max_iterations,
                                          frame_size,
                                          n));
      }

      ldpc_bit_flip_decoder_impl::ldpc_bit_flip_decoder_impl(
                        LDPC_par_chk_mtrx_impl parity_check_matrix,
                        unsigned int max_iterations,
                        unsigned int frame_size,
                        unsigned int n)
        : generic_decoder("LDPC bit flip decoder")
      {
        if(frame_size < 1)
          throw std::runtime_error("ldpc_bit_flip_decoder: frame_size must be > 0");
        if(n < 1)
          throw std::runtime_error("ldpc_bit_flip_decoder: n must be > 0");

        // Set frame size to k, the # of bits in the information word
        // All buffers and settings will be based on this value.
        d_max_frame_size = frame_size;
        set_frame_size(frame_size);
        // Number of bits in the transmitted codeword
        d_n = n;
        // Maximum number of iterations in the decoding algorithm
        d_max_iterations = max_iterations;
        // LDPC parity check matrix to use for decoding
        d_H = parity_check_matrix;
      }

      ldpc_bit_flip_decoder_impl::~ldpc_bit_flip_decoder_impl()
      {
        // free memory here
      }

      int
      ldpc_bit_flip_decoder_impl::get_output_size()
      {
        return d_frame_size;
      }
      
      int
      ldpc_bit_flip_decoder_impl::get_input_size()
      {
        return d_n;
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
        return static_cast<double>(d_frame_size)/d_n;
      }

      void
      ldpc_bit_flip_decoder_impl::generic_work(void *inbuffer,
                                               void *outbuffer)
      {
        const float *in = (const float*)inbuffer;
        unsigned char *out = (unsigned char *) outbuffer;

        // Algorithm: 

        // 1. Check syndrome. If codeword is not valid, continue.
        // 2. While codeword is not valid and iterations < max: 
        //      For each of the n bits in the codeword, determine how
        //      many of the unsatisfied parity checks involve that
        //      bit. To do this, first find the nonzero entries in
        //      the syndrome. The entry numbers correspond to the
        //      rows of entry in H. Second, for each bit, determine
        //      how many of the unsatisfied parity checks involve
        //      this bit and store this count in an array. Next, 
        //      determine which bit(s) is  associated with the most
        //      unsatisfied parity checks, and flip it/them. Check 
        //      the syndrome.
        // 3. After finding the valid codeword, extract the info word
        // 4. Assign the info word to the output.
      }      

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

