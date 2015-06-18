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


#ifndef INCLUDED_POLAR_FEC_DECODER_SC_LIST_H
#define INCLUDED_POLAR_FEC_DECODER_SC_LIST_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/polar_decoder_common.h>

namespace gr {
  namespace fec {
    namespace polar {
      class scl_list;
      class path;
    }

    /*!
     * \brief implements a successive cancellation list decoder for polar codes
     * decoder is based on Tal, Vardy "List Decoding of Polar Codes", 2012
     * LLR version: Balatsoukas-Stimming, Parizi, Burg "LLR-based Successive Cancellation List Decoding of Polar Codes", 2015
     *
     */
    class FEC_API polar_decoder_sc_list : public polar_decoder_common
    {
    public:
      static generic_decoder::sptr make(int max_list_size, int block_size, int num_info_bits, std::vector<int> frozen_bit_positions, std::vector<char> frozen_bit_values, bool is_packed = false);
      ~polar_decoder_sc_list();

      // FECAPI
      void generic_work(void *in_buffer, void *out_buffer);

    private:
      polar_decoder_sc_list(int max_list_size, int block_size, int num_info_bits, std::vector<int> frozen_bit_positions, std::vector<char> frozen_bit_values, bool is_packed);

      unsigned int d_max_list_size;
      polar::scl_list* d_scl;
      unsigned int d_frozen_bit_counter;

      void decode_list();
      void calculate_next_llr_in_paths(int u_num);
      void calculate_next_llr(polar::path* current_path, int u_num);
      void update_with_frozenbit(const int u_num);

    };
  } // namespace fec
} // namespace gr

#endif /* INCLUDED_POLAR_FEC_DECODER_SC_LIST_H */

