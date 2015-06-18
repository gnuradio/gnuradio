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
#include <gnuradio/fec/polar_decoder_sc_list.h>
#include <volk/volk.h>
#include <scl_list.h>

#include <cmath>
#include <algorithm>

namespace gr
{
  namespace fec
  {

    generic_decoder::sptr
    polar_decoder_sc_list::make(int max_list_size, int block_size, int num_info_bits,
                                std::vector<int> frozen_bit_positions,
                                std::vector<char> frozen_bit_values, bool is_packed)
    {
      return generic_decoder::sptr(
          new polar_decoder_sc_list(max_list_size, block_size, num_info_bits, frozen_bit_positions,
                                    frozen_bit_values, is_packed));
    }

    polar_decoder_sc_list::polar_decoder_sc_list(int max_list_size, int block_size,
                                                 int num_info_bits,
                                                 std::vector<int> frozen_bit_positions,
                                                 std::vector<char> frozen_bit_values,
                                                 bool is_packed) :
            polar_decoder_common(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values,
                                 is_packed), d_max_list_size(max_list_size), d_frozen_bit_counter(0)
    {
      d_scl = new polar::scl_list(max_list_size, block_size, block_power());
    }

    polar_decoder_sc_list::~polar_decoder_sc_list()
    {
      delete d_scl;
    }

    void
    polar_decoder_sc_list::generic_work(void* in_buffer, void* out_buffer)
    {
      const float *in = (const float*) in_buffer;
      unsigned char *out = (unsigned char*) out_buffer;
      polar::path* init_path = d_scl->initial_path();
      initialize_llr_vector(init_path->llr_vec, in);
      memset(init_path->u_vec, 0, sizeof(unsigned char) * block_size() * (block_power() + 1));
      decode_list();
      const polar::path* temp = d_scl->optimal_path();
      extract_info_bits(out, temp->u_vec);
    }

    void
    polar_decoder_sc_list::decode_list()
    {
      d_frozen_bit_counter = 0;
      for(int i = 0; i < block_size(); i++){
        calculate_next_llr_in_paths(i);
      }

    }

    void
    polar_decoder_sc_list::calculate_next_llr_in_paths(int u_num)
    {
      for(unsigned int i = 0; i < d_scl->active_size(); i++){
        polar::path* current_path = d_scl->next_active_path();
        calculate_next_llr(current_path, u_num);
      }

      // 1. if frozen bit, update with known value
      if(d_frozen_bit_counter < d_frozen_bit_positions.size() && u_num == d_frozen_bit_positions.at(d_frozen_bit_counter)){
        update_with_frozenbit(u_num);
      }
      // 2. info bit
      else{
        d_scl->set_info_bit(u_num);
      }
    }

    void
    polar_decoder_sc_list::update_with_frozenbit(const int u_num)
    {
      unsigned char frozen_bit = d_frozen_bit_values[d_frozen_bit_counter];
      d_scl->set_frozen_bit(frozen_bit, u_num);
      d_frozen_bit_counter++;
    }

    void
    polar_decoder_sc_list::calculate_next_llr(polar::path* current_path, int u_num)
    {
      butterfly(current_path->llr_vec, 0, current_path->u_vec, u_num);
    }
  } /* namespace fec */
} /* namespace gr */


