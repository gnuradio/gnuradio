/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FEC_CCSDS_DECODER_H
#define INCLUDED_FEC_CCSDS_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/fec/cc_common.h>
#include <map>
#include <string>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief Convolutional Code Decoding class.
       * \ingroup error_coding_blk
       *
       * \details
       * This class performs convolutional decoding via the Viterbi
       * algorithm specific to the CCSDS code:
       *
       * \li K = 7
       * \li rate = 1/2
       * \li polynomials = [109, 79]
       *
       * This is the well-known convolutional part of the Voyager
       * code. See the discussion in gr::fec::code::cc_decoder for
       * the current state of convolutional decoding.
       *
       * Currently, this block is transitional and only implements
       * parts of the FECAPI.
       *
       * The encoder is set up wtih a number of bits per frame in the
       * constructor. When not being used in a tagged stream mode,
       * this encoder will only process frames of the length provided
       * here. If used in a tagged stream block, this setting becomes
       * the maximum allowable frame size that the block may process.
       */
      class FEC_API ccsds_decoder : virtual public generic_decoder
      {
      public:

        /*!
         * Build a convolutional code decoding FECAPI object.
         *
         * \param frame_size Number of bits per frame. If using in the
         *        tagged stream style, this is the maximum allowable
         *        number of bits per frame.
         * \param start_state Initialization state of the shift register.
         * \param end_state Ending state of the shift register.
         * \param mode cc_mode_t mode of the encoding.
         */
        static generic_decoder::sptr make
          (int frame_size,
           int start_state=0, int end_state=-1,
           cc_mode_t mode=CC_STREAMING);

        /*!
         * Sets the uncoded frame size to \p frame_size. If \p
         * frame_size is greater than the value given to the
         * constructor, the frame size will be capped by that initial
         * value and this function will return false. Otherwise, it
         * returns true.
         */
        virtual bool set_frame_size(unsigned int frame_size) = 0;

        /*!
         * Returns the coding rate of this encoder.
         */
        virtual double rate() = 0;
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CCSDS_DECODER_H */
