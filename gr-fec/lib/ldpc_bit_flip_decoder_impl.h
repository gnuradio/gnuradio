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
#include <gnuradio/fec/ldpc_R_U_mtrx.h>

namespace gr {
  namespace fec {
    namespace code {

      class FEC_API ldpc_bit_flip_decoder_impl : public ldpc_bit_flip_decoder
      {
      private:
        // Plug into the generic FEC API:
        int get_input_size();   // n, # of bits in the received block
        int get_output_size();  // k, # of bits in the info word
        unsigned int d_frame_size;

        // LDPC parity check matrix object to use for decoding
        ldpc_R_U_mtrx *d_H;
        // Maximum number of iterations to do in decoding algorithm
        unsigned int d_max_iterations;

      public:
        ldpc_bit_flip_decoder_impl(ldpc_R_U_mtrx *H_obj,
                                   unsigned int max_iter=100);
        ~ldpc_bit_flip_decoder_impl();

        void generic_work(void *inbuffer, void *outbuffer);
        bool set_frame_size(unsigned int frame_size);
        double rate();        
      };
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_IMPL_H */