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


#ifndef INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_IMPL_H
#define INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_IMPL_H

#include <gnuradio/fec/ldpc_bit_flip_decoder.h>

namespace gr {
  namespace fec {
    namespace code {

      class FEC_API ldpc_bit_flip_decoder_impl : public ldpc_bit_flip_decoder
      {
      private:
        // Plug into the generic FEC API:
        int get_input_size();   // n, # of bits in the rc'd block.
        int get_output_size();  // k, # of bits in the info word

        // Everything else:
        // LDPC parity check matrix to use for decoding
        LDPC_parity_check_matrix d_H;
        // Maximum number of iterations to do in decoding algorithm
        unsigned int d_max_iterations;
        // Number of bits in the information word
        unsigned int d_frame_size;
        // Number of bits in the transmitted codeword block
        unsigned int d_n;
        // Function to calculate the syndrome 
        //bool calc_syndrome(LDPC_parity_check_matrix H, <add the codeword here> );

      public:
        ldpc_bit_flip_decoder_impl(
                       LDPC_parity_check_matrix parity_check_matrix, 
                       unsigned int max_iterations=100, 
                       unsigned int frame_size, 
                       unsigned int n);
        ~ldpc_bit_flip_decoder_impl();

        void generic_work(void *inbuffer, void *outbuffer);
        bool set_frame_size(unsigned int frame_size);
        double rate();        
      };
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_IMPL_H */