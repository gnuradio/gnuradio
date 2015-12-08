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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/fec/polar_decoder_sc_systematic.h>
#include <volk/volk.h>

namespace gr {
  namespace fec {
    namespace code {

      generic_decoder::sptr
      polar_decoder_sc_systematic::make(int block_size, int num_info_bits,
                                        std::vector<int> frozen_bit_positions)
      {
        return generic_decoder::sptr(
            new polar_decoder_sc_systematic(block_size, num_info_bits, frozen_bit_positions));
      }

      polar_decoder_sc_systematic::polar_decoder_sc_systematic(
          int block_size, int num_info_bits, std::vector<int> frozen_bit_positions) :
              polar_decoder_common(block_size, num_info_bits, frozen_bit_positions, std::vector<char>())
      {
        d_llr_vec = (float*) volk_malloc(sizeof(float) * block_size * (block_power() + 1), volk_get_alignment());
        memset(d_llr_vec, 0, sizeof(float) * block_size * (block_power() + 1));
        d_u_hat_vec = (unsigned char*) volk_malloc(block_size * (block_power() + 1), volk_get_alignment());
        memset(d_u_hat_vec, 0, sizeof(unsigned char) * block_size * (block_power() + 1));
        d_frame_vec = (unsigned char*) volk_malloc(block_size, volk_get_alignment());
        memset(d_frame_vec, 0, sizeof(unsigned char) * block_size);
      }

      polar_decoder_sc_systematic::~polar_decoder_sc_systematic()
      {
      }

      void
      polar_decoder_sc_systematic::generic_work(void* in_buffer, void* out_buffer)
      {
        const float *in = (const float*) in_buffer;
        unsigned char *out = (unsigned char*) out_buffer;

        initialize_decoder(d_u_hat_vec, d_llr_vec, in);
        sc_decode(d_llr_vec, d_u_hat_vec);
        volk_encode_block(d_frame_vec, d_u_hat_vec);
        extract_info_bits_reversed(out, d_frame_vec);
      }

      void
      polar_decoder_sc_systematic::sc_decode(float* llrs, unsigned char* u)
      {
        for(int i = 0; i < block_size(); i++){
          butterfly(llrs, u, 0, i, i);
          u[i] = retrieve_bit_from_llr(llrs[i], i);
        }
      }

      unsigned char
      polar_decoder_sc_systematic::retrieve_bit_from_llr(float llr, const int pos)
      {
        if(is_frozen_bit(pos)){
          return next_frozen_bit();
        }
        return llr_bit_decision(llr);
      }

      void
      polar_decoder_sc_systematic::extract_info_bits_reversed(unsigned char* outbuf,
                                                              const unsigned char* inbuf)
      {
        for(int i = 0; i < num_info_bits(); i++){
          *outbuf++ = inbuf[d_info_bit_positions_reversed[i]];
        }
      }

    } // namespace code
  } /* namespace fec */
} /* namespace gr */

