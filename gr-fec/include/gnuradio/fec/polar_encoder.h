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
    namespace code {

      /*!
       * \brief POLAR encoder
       * for basic details see 'polar_common' class.
       * \ingroup error_coding_blk
       *
       * \details
       * expects values with MSB first. It needs a full information word and encodes it in one pass.
       * Output is a codeword of block_size.
       */
      class FEC_API polar_encoder : public generic_encoder, public polar_common
      {
      public:
        /*!
         * Factory for a polar code encoder object.
         *
         * \param block_size defines the codeword size. It MUST be a
         *        power of 2.
         * \param num_info_bits represents the number of information
         *        bits in a block. Also called frame_size.
         * \param frozen_bit_positions is an integer vector which
         *        defines the position of all frozen bits in a block.
         *        Its size MUST be equal to block_size - num_info_bits.
         *        Also it must be sorted and every position must only
         *        occur once.
         * \param frozen_bit_values holds an unpacked byte for every
         *        frozen bit position. It defines if a frozen bit is
         *        fixed to '0' or '1'. Defaults to all ZERO.
         * \param is_packed choose 1 active bit/byte or 8 active
         *        bit/byte. if false, VOLK polar encoder is used.
         */
        static generic_encoder::sptr make(int block_size, int num_info_bits,
                                          std::vector<int> frozen_bit_positions,
                                          std::vector<char> frozen_bit_values,
                                          bool is_packed = false);
        ~polar_encoder();

        // FECAPI
        void generic_work(void *in_buffer, void *out_buffer);
        double rate(){return (1.0 * get_input_size() / get_output_size());};
        int get_input_size(){return num_info_bits() / (d_is_packed ? 8 : 1);};
        int get_output_size(){return block_size() / (d_is_packed ? 8 : 1);};
        bool set_frame_size(unsigned int frame_size){return false;};
        const char* get_input_conversion(){return d_is_packed ? "pack" : "none";};
        const char* get_output_conversion(){return d_is_packed ? "packed_bits" : "none";};

      private:
        polar_encoder(int block_size, int num_info_bits,
                      std::vector<int>& frozen_bit_positions,
                      std::vector<char>& frozen_bit_values, bool is_packed);
        bool d_is_packed;

        // c'tor method for packed algorithm setup.
        void setup_frozen_bit_inserter();

        // methods insert input bits and frozen bits into packed array for encoding
        unsigned char* d_frozen_bit_prototype; // packed frozen bits are written onto it and later copies are used.
        void insert_packed_frozen_bits_and_reverse(unsigned char* target,
                                                   const unsigned char* input) const;
        void insert_unpacked_bit_into_packed_array_at_position(unsigned char* target,
                                                               const unsigned char bit,
                                                               const int pos) const;
        void insert_packet_bit_into_packed_array_at_position(unsigned char* target,
                                                             const unsigned char bit,
                                                             const int target_pos,
                                                             const int bit_pos) const;

        // packed encoding methods
        void encode_vector_packed(unsigned char* target) const;
        void encode_vector_packed_subbyte(unsigned char* target) const;
        void encode_packed_byte(unsigned char* target) const;
        void encode_vector_packed_interbyte(unsigned char* target) const;
      };

    } // namespace code
  } // namespace fec
} // namespace gr

#endif /* INCLUDED_FEC_POLAR_ENCODER_H */
