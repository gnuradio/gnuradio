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


#ifndef INCLUDED_FEC_POLAR_ENCODER_H
#define INCLUDED_FEC_POLAR_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/fec/polar_common.h>

namespace gr {
  namespace fec {

    /*!
     * \brief POLAR encoder
     *
     */
    class FEC_API polar_encoder : public generic_encoder, public polar_common
    {
    public:
      static generic_encoder::sptr make(int block_size, int num_info_bits, std::vector<int> frozen_bit_positions, std::vector<char> frozen_bit_values, bool is_packed = false);
      ~polar_encoder();

      // FECAPI
      void generic_work(void *in_buffer, void *out_buffer);
      double rate(){return (1.0 * get_input_size() / get_output_size());};
      int get_input_size(){return num_info_bits() / (is_packed() ? 8 : 1);};
      int get_output_size(){return block_size() / (is_packed() ? 8 : 1);};
      bool set_frame_size(unsigned int frame_size){return false;};
      const char* get_input_conversion(){return is_packed() ? "pack" : "none";};
      const char* get_output_conversion(){return is_packed() ? "packed_bits" : "none";};

    private:
      polar_encoder(int block_size, int num_info_bits, std::vector<int> frozen_bit_positions, std::vector<char> frozen_bit_values, bool is_packed);
      std::vector<int> d_frozen_bit_positions;
      std::vector<int> d_info_bit_positions;
      std::vector<char> d_frozen_bit_values;

      // for unpacked bits an 'easier-to-grasp' algorithm.
      void insert_frozen_bits(unsigned char* target, const unsigned char* input);
      void bit_reverse_vector(unsigned char* target, const unsigned char* input);
      void encode_vector(unsigned char* target);

      // c'tor method for packed algorithm setup.
      void setup_frozen_bit_inserter();

      // methods insert input bits and frozen bits into packed array for encoding
      unsigned char* d_block_array; // use for encoding
      unsigned char* d_frozen_bit_prototype; // packed frozen bits are written onto it and later copies are used.
      void insert_unpacked_frozen_bits_and_reverse(unsigned char* target, const unsigned char* input) const;
      void insert_packed_frozen_bits_and_reverse(unsigned char* target, const unsigned char* input) const;
      void insert_unpacked_bit_into_packed_array_at_position(unsigned char* target, const unsigned char bit, const int pos) const;
      void insert_packet_bit_into_packed_array_at_position(unsigned char* target, const unsigned char bit, const int target_pos, const int bit_pos) const;

      // packed encoding methods
      void encode_vector_packed(unsigned char* target) const;
      void encode_vector_packed_subbyte(unsigned char* target) const;
      void encode_packed_byte(unsigned char* target) const;
      void encode_vector_packed_interbyte(unsigned char* target) const;

    };

  } // namespace fec
} // namespace gr

#endif /* INCLUDED_FEC_POLAR_ENCODER_H */

