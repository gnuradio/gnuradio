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
    namespace code {

      namespace polar {
        class scl_list;
        class path;
      }

      /*!
       * \brief Successive cancellation list (SCL) decoder for polar
       * codes.
       *
       * \details
       * Decoder is based on Tal, Vardy "List Decoding of Polar Codes",
       * 2012 LLR version: Balatsoukas-Stimming, Parizi, Burg "LLR-based
       * Successive Cancellation List Decoding of Polar Codes", 2015.
       *
       * Block expects float input with bits mapped 1 --> 1, 0 --> -1
       * Or: f = 2.0 * bit - 1.0
       */
      class FEC_API polar_decoder_sc_list : public polar_decoder_common
      {
      public:
        /*!
         * \param max_list_size caps number of followed decoding paths.
         * \param block_size codeword size. MUST be a power of 2.
         * \param num_info_bits represents the number of information
         *        bits in a block. Also called frame_size. <= block_size
         * \param frozen_bit_positions is an integer vector which
         *        defines the position of all frozen bits in a block.
         *        Its size MUST be equal to block_size - num_info_bits.
         *        Also it must be sorted and every position must only
         *        occur once.
         * \param frozen_bit_values holds an unpacked byte for every
         *        frozen bit position. It defines if a frozen bit is
         *        fixed to '0' or '1'. Defaults to all ZERO.
         */
        static generic_decoder::sptr make(int max_list_size, int block_size,
                                          int num_info_bits, std::vector<int> frozen_bit_positions,
                                          std::vector<char> frozen_bit_values);
        ~polar_decoder_sc_list();

        // FECAPI
        void generic_work(void *in_buffer, void *out_buffer);

      private:
        polar_decoder_sc_list(int max_list_size, int block_size,
                              int num_info_bits, std::vector<int> frozen_bit_positions,
                              std::vector<char> frozen_bit_values);

        polar::scl_list* d_scl;

        void initialize_list(const float* in_buf);
        const unsigned char* decode_list();
        void decode_bit(const int u_num);
        void calculate_llrs_for_list(const int u_num);
        void set_bit_in_list(const int u_num);
      };

    } // namespace code
  } // namespace fec
} // namespace gr

#endif /* INCLUDED_POLAR_FEC_DECODER_SC_LIST_H */
