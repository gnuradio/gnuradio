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


#ifndef INCLUDED_FEC_POLAR_ENCODER_SYSTEMATIC_H
#define INCLUDED_FEC_POLAR_ENCODER_SYSTEMATIC_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/fec/polar_common.h>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief systematic POLAR encoder
       * for basic details see 'polar_common' class.
       * \ingroup error_coding_blk
       *
       * \details
       * expects values with MSB first. It needs a full information word and encodes it in one pass.
       * Output is a codeword of block_size.
       *
       * Systematic encoding indicates that the info bit values are present in the codeword.
       * 'info_bit_positions' may be obtained by ordering all non frozen_bit_positions in increasing order.
       * One may extract them at their positions after a bit reversal operation.
       * encoder -> decoder chain would need additional bit-reversal after encoding + before decoding.
       * This is unnecessary.
       */
      class FEC_API polar_encoder_systematic: public generic_encoder, public polar_common
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
         *        occur once. Frozen bit values will be set to ZERO!
         */
        static generic_encoder::sptr
        make(int block_size, int num_info_bits, std::vector<int> frozen_bit_positions);

        // FECAPI
        void
        generic_work(void *in_buffer, void *out_buffer);
        double
        rate()
        {
          return (1.0 * get_input_size() / get_output_size());
        }
        ;
        int
        get_input_size()
        {
          return num_info_bits();
        }
        ;
        int
        get_output_size()
        {
          return block_size();
        }
        ;
        bool
        set_frame_size(unsigned int frame_size)
        {
          return false;
        }
        ;

        ~polar_encoder_systematic();
      private:
        polar_encoder_systematic(int block_size, int num_info_bits,
                                 std::vector<int> frozen_bit_positions);

        void bit_reverse_and_reset_frozen_bits(unsigned char *outbuf, const unsigned char *inbuf);
        unsigned char* d_volk_syst_intermediate;
      };

    } // namespace code
  } // namespace fec
} // namespace gr

#endif /* INCLUDED_FEC_POLAR_ENCODER_SYSTEMATIC_H */

