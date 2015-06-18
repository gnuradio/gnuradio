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
#include <gnuradio/fec/polar_encoder.h>
#include <cmath>
#include <stdexcept>
#include <volk/volk.h>

#include <gnuradio/blocks/pack_k_bits.h>
#include <gnuradio/blocks/unpack_k_bits.h>

namespace gr
{
  namespace fec
  {

    generic_encoder::sptr
    polar_encoder::make(int block_size, int num_info_bits, std::vector<int> frozen_bit_positions,
                        std::vector<char> frozen_bit_values, bool is_packed)
    {
      return generic_encoder::sptr(
          new polar_encoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values,
                            is_packed));
    }

    polar_encoder::polar_encoder(int block_size, int num_info_bits,
                                 std::vector<int> frozen_bit_positions,
                                 std::vector<char> frozen_bit_values, bool is_packed) :
        polar_common(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values, is_packed),
            d_frozen_bit_positions(frozen_bit_positions),
            d_frozen_bit_values(frozen_bit_values)
    {
      unsigned int num_frozen_bits = block_size - num_info_bits;

      while(d_frozen_bit_values.size() < num_frozen_bits) {
        d_frozen_bit_values.push_back(0);
      }

      setup_frozen_bit_inserter();
    }

    void
    polar_encoder::setup_frozen_bit_inserter()
    {
      d_block_array = (unsigned char*) volk_malloc(block_size() >> 3, volk_get_alignment());
      d_frozen_bit_prototype = (unsigned char*) volk_malloc(block_size() >> 3,
                                                            volk_get_alignment());
      memset(d_frozen_bit_prototype, 0, block_size() >> 3);

      for(unsigned int i = 0; i < d_frozen_bit_positions.size(); i++) {
        int rev_pos = (int) bit_reverse((long) d_frozen_bit_positions.at(i), block_power());
        unsigned char frozen_bit = (unsigned char) d_frozen_bit_values.at(i);
        insert_unpacked_bit_into_packed_array_at_position(d_frozen_bit_prototype, frozen_bit,
                                                          rev_pos);
      }

      std::vector<int> temp_vec = info_bit_position_vector();
      for(unsigned int i = 0; i < temp_vec.size(); i++){
        d_info_bit_positions.push_back((int) bit_reverse((long) temp_vec.at(i), block_power()));
      }

      if((int) d_info_bit_positions.size() != num_info_bits()) {
        throw std::runtime_error("number of info bit positions MUST equal num_info_bits (K)!");
      }
    }

    polar_encoder::~polar_encoder()
    {
      volk_free(d_block_array);
      volk_free(d_frozen_bit_prototype);
    }

    void
    polar_encoder::generic_work(void* in_buffer, void* out_buffer)
    {
      const unsigned char *in = (const unsigned char*) in_buffer;
      unsigned char *out = (unsigned char*) out_buffer;

      if(is_packed()) {
        insert_packed_frozen_bits_and_reverse(out, in);
        encode_vector_packed(out);
      }
      else {
        insert_unpacked_frozen_bits_and_reverse(d_block_array, in);
        encode_vector_packed(d_block_array);
        unpacker()->unpack(out, d_block_array, block_size() >> 3);
      }

//      insert_frozen_bits(d_block_array, in);
//      bit_reverse_vector(out, d_block_array);
//      encode_vector(out);
    }

    void
    polar_encoder::encode_vector_packed(unsigned char* target) const
    {
      encode_vector_packed_subbyte(target);
      encode_vector_packed_interbyte(target);
    }

    void
    polar_encoder::encode_vector_packed_subbyte(unsigned char* target) const
    {
      int num_bytes_per_block = block_size() >> 3;
      while(num_bytes_per_block) {
        encode_packed_byte(target);
        ++target;
        --num_bytes_per_block;
      }
    }

    void
    polar_encoder::encode_packed_byte(unsigned char* target) const
    {
      // this method only produces correct results if d_block_size > 4.
      // this is assumed to be the case.
      *target ^= 0xaa & (*target << 1);
      *target ^= 0xcc & (*target << 2);
      *target ^= *target << 4;
    }

    void
    polar_encoder::encode_vector_packed_interbyte(unsigned char* target) const
    {
      int branch_byte_size = 1;
      unsigned char* pos;
      int n_branches = block_size() >> 4;
      int byte = 0;
      for(int stage = 3; stage < block_power(); ++stage) {
        pos = target;

        for(int branch = 0; branch < n_branches; ++branch) {

          byte = 0;
          while(byte < branch_byte_size) {
            *pos ^= *(pos + branch_byte_size);
            ++pos;
            ++byte;
          }

          pos += branch_byte_size;
        }

        n_branches >>= 1;
        branch_byte_size <<= 1;
      }
    }

    void
    polar_encoder::insert_unpacked_frozen_bits_and_reverse(unsigned char* target,
                                                           const unsigned char* input) const
    {
      memcpy(target, d_frozen_bit_prototype, block_size() >> 3);
      const int* info_bit_positions_ptr = &d_info_bit_positions[0];
      const unsigned char* end_input = input + num_info_bits();
      int bit_pos = 7;
      while(input < end_input) {
        insert_packet_bit_into_packed_array_at_position(target, *input++, *info_bit_positions_ptr++,
                                                        bit_pos);
      }
    }

    void
    polar_encoder::insert_packed_frozen_bits_and_reverse(unsigned char* target,
                                                         const unsigned char* input) const
    {
      memcpy(target, d_frozen_bit_prototype, block_size() >> 3);
      const int* info_bit_positions_ptr = &d_info_bit_positions[0];
      int bit_num = 0;
      unsigned char byte = *input;
      int bit_pos;
      while(bit_num < num_info_bits()) {
        bit_pos = *info_bit_positions_ptr++;
        insert_packet_bit_into_packed_array_at_position(target, byte, bit_pos, bit_num % 8);
        ++bit_num;
        if(bit_num % 8 == 0) {
          ++input;
          byte = *input;
        }
      }
    }

    void
    polar_encoder::insert_unpacked_bit_into_packed_array_at_position(unsigned char* target,
                                                                     const unsigned char bit,
                                                                     const int pos) const
    {
      int byte_pos = pos >> 3;
      int bit_pos = pos & 0x7;
      *(target + byte_pos) ^= bit << (7 - bit_pos);
    }

    void
    polar_encoder::insert_packet_bit_into_packed_array_at_position(unsigned char* target,
                                                                   const unsigned char bit,
                                                                   const int target_pos,
                                                                   const int bit_pos) const
    {
      insert_unpacked_bit_into_packed_array_at_position(target, (bit >> (7 - bit_pos)) & 0x01,
                                                        target_pos);
    }

    void
    polar_encoder::insert_frozen_bits(unsigned char* target, const unsigned char* input)
    {
      int frozen_num = 0;
      int num_frozen_bits = block_size() - num_info_bits();
      int info_num = 0;
      for(int i = 0; i < block_size(); i++) {
        if(frozen_num < num_frozen_bits && d_frozen_bit_positions.at(frozen_num) == i) {
          target[i] = d_frozen_bit_values.at(frozen_num);
          frozen_num++;
        }
        else {
          target[i] = input[info_num];
          info_num++;
        }
      }
    }

    void
    polar_encoder::bit_reverse_vector(unsigned char* target, const unsigned char* input)
    {
      for(int i = 0; i < block_size(); i++) {
        target[bit_reverse(long(i), block_power())] = input[i];
      }
    }

    void
    polar_encoder::encode_vector(unsigned char* target)
    {
      for(int stage = 0; stage < block_power(); stage++) {
        int n_branches = pow(2, stage);
        int branch_elements = block_size() / (2 * n_branches);
        for(int branch = 0; branch < n_branches; branch++) {
          for(int e = 0; e < branch_elements; e++) {
            int pos = branch * branch_elements * 2 + e;
            target[pos] ^= target[pos + branch_elements];
          }
        }
      }
    }

  } /* namespace fec */
} /* namespace gr */

