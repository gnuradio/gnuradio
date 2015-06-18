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


#ifndef INCLUDED_FEC_POLAR_DECODER_COMMON_H
#define INCLUDED_FEC_POLAR_DECODER_COMMON_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/fec/polar_common.h>

namespace gr {
  namespace fec {

    /*!
     * \brief Class holds common methods and attributes for different decoder implementations
     *
     */
    class FEC_API polar_decoder_common : public generic_decoder, public polar_common
    {
    public:
      polar_decoder_common(int block_size, int num_info_bits, std::vector<int> frozen_bit_positions, std::vector<char> frozen_bit_values, bool is_packed);
      ~polar_decoder_common();

      // FECAPI
      double rate(){return (1.0 * get_input_size() / get_output_size());};
      int get_input_size(){return block_size() / (is_packed() ? 8 : 1);};
      int get_output_size(){return num_info_bits() / (is_packed() ? 8 : 1);};
      bool set_frame_size(unsigned int frame_size){return false;};
      const char* get_output_conversion() {return "none";};

    private:
      const float D_LLR_FACTOR;

    protected:
      // calculate LLRs for stage
      float llr_odd(const float la, const float lb) const;
      float llr_even(const float la, const float lb, const unsigned char f) const;
      unsigned char llr_bit_decision(const float llr) const {return (llr < 0.0f) ? 1 : 0;};

      // preparation for decoding
      void initialize_llr_vector(float* llrs, const float* input);
      // basic algorithm methods
      void butterfly(float* llrs, const int stage, unsigned char* u, const int u_num);
      void even_u_values(unsigned char* u_even, const unsigned char* u, const int u_num);
      void odd_xor_even_values(unsigned char* u_xor, const unsigned char* u, const int u_num);
      void demortonize_values(unsigned char* u);

      void extract_info_bits(unsigned char* output, const unsigned char* input) const;

      static void insert_bit_at_pos(unsigned char* u, const unsigned char ui, const unsigned int pos){u[pos >> 3] ^= ui << (7 - (pos % 8));};
      static unsigned char fetch_bit_at_pos(const unsigned char* u, const unsigned int pos){return (u[pos >> 3] >> (7 - (pos % 8))) & 0x01;};

      // info shared among all implementations.
      std::vector<int> d_frozen_bit_positions;
      std::vector<int> d_info_bit_positions;
      std::vector<char> d_frozen_bit_values;

      // helper functions.
      void print_pretty_llr_vector(const float* llr_vec) const;

    };

  } // namespace fec
} // namespace gr

#endif /* INCLUDED_FEC_POLAR_DECODER_COMMON_H */

