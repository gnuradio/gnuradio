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
#include <gnuradio/fec/polar_common.h>
#include <volk/volk.h>

#include <gnuradio/blocks/pack_k_bits.h>
#include <gnuradio/blocks/unpack_k_bits.h>

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace gr {
  namespace fec {
    namespace code {

      polar_common::polar_common(int block_size, int num_info_bits,
                                 std::vector<int> frozen_bit_positions,
                                 std::vector<char> frozen_bit_values) :
        d_frozen_bit_positions(frozen_bit_positions), d_frozen_bit_values(frozen_bit_values),
        d_block_size(block_size), d_block_power((int) log2(float(block_size))),
        d_num_info_bits(num_info_bits)
      {
        if(pow(2, d_block_power) != d_block_size){
          throw std::runtime_error("block_size MUST be a power of 2!");
        }

        unsigned int num_frozen_bits = d_block_size - d_num_info_bits;
        if(num_frozen_bits != d_frozen_bit_positions.size()){
          throw std::runtime_error(
                                   "number of frozen bit positions must equal block_size - num_info_bits");
        }

        // According to papers frozen bits default to '0'.
        while(d_frozen_bit_values.size() < num_frozen_bits){
          d_frozen_bit_values.push_back(0);
        }
        initialize_info_bit_position_vector();
        setup_volk_vectors();
        setup_info_bit_positions_reversed();

        d_unpacker = new gr::blocks::kernel::unpack_k_bits(8);
      }

      void
      polar_common::initialize_info_bit_position_vector()
      {
        int num_frozen_bit = 0;
        int frozen_pos = d_frozen_bit_positions.at(num_frozen_bit);
        for(int i = 0; i < d_block_size; i++) {
          if(i != frozen_pos) {
            d_info_bit_positions.push_back((int) i);
          }
          else {
            num_frozen_bit++;
            num_frozen_bit = std::min(num_frozen_bit, (int) (d_frozen_bit_positions.size() - 1));
            frozen_pos = d_frozen_bit_positions.at(num_frozen_bit);
          }
        }

        if((int) d_info_bit_positions.size() != num_info_bits()) {
          throw std::runtime_error("polar_common: number of info bit positions MUST equal num_info_bits (K)!");
        }
      }

      void
      polar_common::setup_info_bit_positions_reversed()
      {
        for(unsigned int i = 0; i < d_info_bit_positions.size(); i++){
          d_info_bit_positions_reversed.push_back((int) bit_reverse((long) d_info_bit_positions.at(i), block_power()));
        }

        if((int) d_info_bit_positions_reversed.size() != num_info_bits()) {
          throw std::runtime_error("polar_encoder: number of info bit positions MUST equal num_info_bits (K)!");
        }
      }

      void
      polar_common::setup_volk_vectors()
      {
        int nfrozen = block_size() - num_info_bits();
        d_volk_temp = (unsigned char*) volk_malloc(sizeof(unsigned char) * block_size(), volk_get_alignment());
        d_volk_frozen_bit_mask = (unsigned char*) volk_malloc(sizeof(unsigned char) * block_size(), volk_get_alignment());
        d_volk_frozen_bits = (unsigned char*) volk_malloc(sizeof(unsigned char) * nfrozen, volk_get_alignment());
        std::copy(d_frozen_bit_values.begin(), d_frozen_bit_values.end(), d_volk_frozen_bits);
        std::fill(d_volk_frozen_bits + d_frozen_bit_values.size(), d_volk_frozen_bits + nfrozen, 0);

        int nfbit = 0;
        for(int i = 0; i < block_size(); i++){
          unsigned char m = 0x00;
          if(nfbit < d_frozen_bit_positions.size() && d_frozen_bit_positions[nfbit] == i){
            m = 0xFF;
            nfbit++;
          }
          d_volk_frozen_bit_mask[i] = m;
        }
      }

      void
      polar_common::volk_encode(unsigned char* out_buf, const unsigned char* in_buf)
      {
        volk_8u_x3_encodepolar_8u_x2(out_buf, d_volk_temp, d_volk_frozen_bit_mask, d_volk_frozen_bits, in_buf, block_size());
      }

      void
      polar_common::volk_encode_block(unsigned char* out_buf, unsigned char* in_buf)
      {
        volk_8u_x2_encodeframepolar_8u(out_buf, in_buf, block_size());
      }

      polar_common::~polar_common()
      {
        delete d_unpacker;

        volk_free(d_volk_temp);
        volk_free(d_volk_frozen_bit_mask);
        volk_free(d_volk_frozen_bits);
      }

      long
      polar_common::bit_reverse(long value, int active_bits) const
      {
        long r = 0;
        for(int i = 0; i < active_bits; i++) {
          r <<= 1;
          r |= value & 1;
          value >>= 1;
        }
        return r;
      }

      void
      polar_common::print_packed_bit_array(const unsigned char* printed_array,
                                           const int num_bytes) const
      {
        int num_bits = num_bytes << 3;
        unsigned char* temp = new unsigned char[num_bits];
        d_unpacker->unpack(temp, printed_array, num_bytes);

        std::cout << "[";
        for(int i = 0; i < num_bits; i++) {
          std::cout << (int) *(temp + i) << " ";
        }
        std::cout << "]" << std::endl;

        delete [] temp;
      }

      void
      polar_common::print_unpacked_bit_array(const unsigned char* bits,
                                             const unsigned int num_bytes) const
      {
        std::cout << "( ";
        for(unsigned int i = 0; i < num_bytes; i++){
          std::cout << (int) *bits++ << ", ";
        }
        std::cout << ")" << std::endl;
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
