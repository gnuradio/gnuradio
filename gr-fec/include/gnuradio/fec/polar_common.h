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


#ifndef INCLUDED_FEC_POLAR_COMMON_H
#define INCLUDED_FEC_POLAR_COMMON_H

#include <gnuradio/fec/api.h>

// Forward declaration for those objects. SWIG doesn't like them to be #include'd.
namespace gr {
  namespace blocks {
    namespace kernel {
      class unpack_k_bits;
    }
  }
}

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief POLAR code common operations and attributes
       * \ingroup error_coding_blk
       *
       * \details
       * Polar codes are based on this paper by Erdal Arikan "Channel
       * Polarization: A Method for Constructing Capacity-Achieving Codes
       * for Symmetric Binary-Input Memoryless Channels", 2009 block
       * holds common information for encoders and decoders. All polar
       * encoder/decoders inherit from polar_common.
       *
       * class holds common info. It is common to all encoders and decoders.
       */
      class FEC_API polar_common
      {
      public:
        /*!
         * \param block_size codeword size. MUST be a power of 2.
         * \param num_info_bits represents the number of information
         *        bits in a block. Also called frame_size. <= block_size
         * \param frozen_bit_positions is an integer vector which
         *        defines the position of all frozen bits in a block.
         *        Its size MUST be equal to block_size - num_info_bits.
         *        Also it must be sorted and every position must only
         *        occur once.
         * \param frozen_bit_values holds an unpacked byte for every
         *        frozen bit position.  It defines if a frozen bit is
         *        fixed to '0' or '1'. Defaults to all ZERO.
         */
        polar_common(int block_size, int num_info_bits,
                     std::vector<int> frozen_bit_positions,
                     std::vector<char> frozen_bit_values);
        ~polar_common();

      protected:
        const int block_size()const {return d_block_size;};
        const int block_power()const {return d_block_power;};
        const int num_info_bits() const {return d_num_info_bits;};

        // helper functions
        long bit_reverse(long value, int active_bits) const;
        void print_packed_bit_array(const unsigned char* printed_array,
                                    const int num_bytes) const;
        void print_unpacked_bit_array(const unsigned char* bits,
                                      const unsigned int num_bytes) const;

        std::vector<int> d_frozen_bit_positions;
        std::vector<char> d_frozen_bit_values;
        std::vector<int> d_info_bit_positions;
        std::vector<int> d_info_bit_positions_reversed;
        void setup_info_bit_positions_reversed();
//        std::vector<int> d_info_bit_positions_reversed;


        // VOLK methods
        void setup_volk_vectors();
        void volk_encode(unsigned char* out_buf, const unsigned char* in_buf);
        void volk_encode_block(unsigned char* out_buf, unsigned char* in_buf);
        unsigned char* d_volk_temp;
        unsigned char* d_volk_frozen_bit_mask;
        unsigned char* d_volk_frozen_bits;

      private:
        int d_block_size; // depending on paper called 'N' or 'm'
        int d_block_power;
        int d_num_info_bits; // mostly abbreviated by 'K'

        void initialize_info_bit_position_vector();

        gr::blocks::kernel::unpack_k_bits *d_unpacker; // convenience for 'print_packed_bit_array' function.
      };

    } //namespace code
  } // namespace fec
} // namespace gr

#endif /* INCLUDED_FEC_POLAR_COMMON_H */
