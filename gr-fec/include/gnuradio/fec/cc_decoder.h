/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FEC_CC_DECODER_H
#define INCLUDED_FEC_CC_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/fec/cc_common.h>
#include <map>
#include <string>

namespace gr {
  namespace fec {
    namespace code {

      typedef void(*conv_kernel)(unsigned char  *Y, unsigned char  *X,
                                 unsigned char *syms, unsigned char *dec,
                                 unsigned int framebits, unsigned int excess,
                                 unsigned char *Branchtab);

      /*!
       * \brief Convolutional Code Decoding class.
       * \ingroup error_coding_blk
       *
       * \details
       * This class performs convolutional decoding via the Viterbi
       * algorithm. While it is set up to take variable values for K,
       * rate, and the polynomials, currently, the block is only
       * capable of handling the following settings:
       *
       * \li K = 7
       * \li rate = 1/2 (given as 2 to the constructor)
       * \li polynomials = [109, 79]
       *
       * This is the well-known convolutional part of the Voyager code
       * implemented in the CCSDS encoder.
       *
       * The intent of having this FECAPI code classes fully
       * parameterizable is to eventually allow it to take on generic
       * settings, much like the cc_encoder class where the CCSDS
       * settings would be a highly-optimized version of this.
       *
       * The encoder is set up wtih a number of bits per frame in the
       * constructor. When not being used in a tagged stream mode,
       * this encoder will only process frames of the length provided
       * here. If used in a tagged stream block, this setting becomes
       * the maximum allowable frame size that the block may process.
       */
      class FEC_API cc_decoder : virtual public generic_decoder
      {
      public:

        /*!
         * Build a convolutional code decoding FECAPI object.
         *
         * \param frame_size Number of bits per frame. If using in the
         *        tagged stream style, this is the maximum allowable
         *        number of bits per frame.
         * \param k Constraint length (K) of the encoder.
         * \param rate Inverse of the coder's rate
         *             (rate=2 means 2 output bits per 1 input).
         * \param polys Vector of polynomials as integers.
         * \param start_state Initialization state of the shift register.
         * \param end_state Ending state of the shift register.
         * \param mode cc_mode_t mode of the encoding.
         * \param padded true if the encoded frame is padded
         *               to the nearest byte.
         */
        static generic_decoder::sptr make
          (int frame_size, int k,
           int rate, std::vector<int> polys,
           int start_state=0, int end_state=-1,
           cc_mode_t mode=CC_STREAMING,
           bool padded=false);

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

#endif /* INCLUDED_FEC_CC_DECODER_H */
