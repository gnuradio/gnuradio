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
#include <gnuradio/fec/polar_decoder_common.h>
#include <volk/volk.h>

#include <cstdio>

namespace gr {
  namespace fec {
    namespace code {

      const float polar_decoder_common::D_LLR_FACTOR = -2.19722458f;

      polar_decoder_common::polar_decoder_common(int block_size, int num_info_bits,
                                                 std::vector<int> frozen_bit_positions,
                                                 std::vector<char> frozen_bit_values) :
        polar_common(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values),
        d_frozen_bit_counter(0)
      {
      }

      polar_decoder_common::~polar_decoder_common()
      {
      }

      void
      polar_decoder_common::initialize_decoder(unsigned char* u, float* llrs, const float* input)
      {
        volk_32f_s32f_multiply_32f(llrs + block_size() * block_power(), input, D_LLR_FACTOR, block_size());
        memset(u, 0, sizeof(unsigned char) * block_size() * block_power());
        d_frozen_bit_counter = 0;
      }

      float
      polar_decoder_common::llr_odd(const float la, const float lb) const
      {
        return copysignf(1.0f, la) * copysignf(1.0f, lb) * std::min(fabs(la), fabs(lb));
      }

      float
      polar_decoder_common::llr_even(const float la, const float lb, const unsigned char f) const
      {
        switch(f){
        case 0:
          return lb + la;
        default:
          return lb - la;
        }
      }

      void
      polar_decoder_common::butterfly(float* llrs, unsigned char* u, const int stage,
                                      const int u_num, const int row)
      {
        butterfly_volk(llrs, u, stage, u_num, row);
      }

      void
      polar_decoder_common::butterfly_generic(float* llrs, unsigned char* u, const int stage,
                                              const int u_num, const int row)
      {
        const int next_stage = stage + 1;
        const int half_stage_size = 0x01 << stage;
        const int stage_size = half_stage_size << 1;
        const bool is_upper_stage_half = row % stage_size < half_stage_size;

        //      // this is a natural bit order impl
        float* next_llrs = llrs + block_size(); // LLRs are stored in a consecutive array.
        float* call_row_llr = llrs + row;

        const int section = row - (row % stage_size);
        const int jump_size = ((row % half_stage_size) << 1) % stage_size;

        const int next_upper_row = section + jump_size;
        const int next_lower_row = next_upper_row + 1;

        const float* upper_right_llr_ptr = next_llrs + next_upper_row;
        const float* lower_right_llr_ptr = next_llrs + next_lower_row;

        if(!is_upper_stage_half){
          const int u_pos = u_num >> stage;
          const unsigned char f = u[u_pos - 1];
          *call_row_llr = llr_even(*upper_right_llr_ptr, *lower_right_llr_ptr, f);
          return;
        }

        if(block_power() > next_stage){
          unsigned char* u_half = u + block_size();
          odd_xor_even_values(u_half, u, u_num);
          butterfly(next_llrs, u_half, next_stage, u_num, next_upper_row);

          even_u_values(u_half, u, u_num);
          butterfly(next_llrs, u_half, next_stage, u_num, next_lower_row);
        }

        *call_row_llr = llr_odd(*upper_right_llr_ptr, *lower_right_llr_ptr);
      }

      void
      polar_decoder_common::butterfly_volk(float* llrs, unsigned char* u, const int stage,
                                           const int u_num, const int row)
      {
        volk_32f_8u_polarbutterfly_32f(llrs, u, block_power(), stage, u_num, row);
      }


      void
      polar_decoder_common::even_u_values(unsigned char* u_even, const unsigned char* u,
                                          const int u_num)
      {
        u++;
        for(int i = 1; i < u_num; i += 2){
          *u_even++ = *u;
          u += 2;
        }
      }

      void
      polar_decoder_common::odd_xor_even_values(unsigned char* u_xor, const unsigned char* u,
                                                const int u_num)
      {
        for(int i = 1; i < u_num; i += 2){
          *u_xor++ = *u ^ *(u + 1);
          u += 2;
        }
      }

      const bool
      polar_decoder_common::is_frozen_bit(const int u_num) const
      {
        return d_frozen_bit_counter < d_frozen_bit_positions.size() && u_num == d_frozen_bit_positions.at(d_frozen_bit_counter);
      }


      const unsigned char
      polar_decoder_common::next_frozen_bit()
      {
        return d_frozen_bit_values[d_frozen_bit_counter++];
      }

      void
      polar_decoder_common::extract_info_bits(unsigned char* output, const unsigned char* input) const
      {
        unsigned int frozenbit_num = 0;
        for(int i = 0; i < block_size(); i++){
          if(frozenbit_num < d_frozen_bit_positions.size() && d_frozen_bit_positions.at(frozenbit_num) == i){
            frozenbit_num++;
          }
          else{
            *output++ = *input;
          }
          input++;
        }
      }

      void
      polar_decoder_common::print_pretty_llr_vector(const float* llr_vec) const
      {
        for(int row = 0; row < block_size(); row++) {
          std::cout << row << "->" << int(bit_reverse(row, block_power())) << ":\t";
          for(int stage = 0; stage < block_power() + 1; stage++) {
            printf("%+4.2f, ", llr_vec[(stage * block_size()) + row]);
          }
          std::cout << std::endl;
        }
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
