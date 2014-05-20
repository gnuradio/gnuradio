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

#ifndef INCLUDED_FEC_CC_ENCODER_H
#define INCLUDED_FEC_CC_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/encoder.h>
#include <gnuradio/fec/cc_common.h>
#include <map>
#include <string>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief Convolutional Code Encoding class.
       * \ingroup error_coding_blk
       *
       * \details
       * This class performs convolutional encoding for unpacked bits
       * for frames of a constant length. This class is general in its
       * application of the convolutional encoding and allows us to
       * specify the constraint length, the coding rate, and the
       * polynomials used in the coding process.
       *
       * The parameter \p k sets the constraint length directly. We
       * set the coding rate by setting \p rate to R given a desired
       * rate of 1/R. That is, for a rate 1/2 coder, we would set \p
       * rate to 2. And the polynomial is specified as a vector of
       * integers, where each integer represents the coding polynomial
       * for a different arm of the code. The number of polynomials
       * given must be the same as the value \p rate.
       *
       * The encoding object holds a shift register that takes in each
       * bit from the input stream and then ANDs the shift register
       * with each polynomial, and places the parity of the result
       * into the output stream. The output stream is therefore also
       * unpacked bits.
       *
       * The encoder is set up with a number of bits per frame in the
       * constructor. When not being used in a tagged stream mode,
       * this encoder will only process frames of the length provided
       * here. If used in a tagged stream block, this setting becomes
       * the maximum allowable frame size that the block may process.
       *
       * The \p mode is a cc_mode_t that specifies how the convolutional
       * encoder will behave and under what conditions.
       *
       * \li 'CC_STREAMING': mode expects an uninterrupted flow of
       * samples into the encoder, and the output stream is
       * continually encoded.
       *
       * \li 'CC_TERMINATED': is a mode designed for packet-based
       * systems. This mode adds rate*(k-1) bits to the output as a
       * way to help flush the decoder.
       *
       * \li 'CC_TAILBITING': is another packet-based method. Instead of
       * adding bits onto the end of the packet, this mode will
       * continue the code between the payloads of packets by
       * pre-initializing the state of the new packet based on the
       * state of the last packet for (k-1) bits.
       *
       * \li 'CC_TRUNCATED': a truncated code always resets the registers
       * to the \p start_state between frames.
       *
       * A common convolutional encoder uses K=7, Rate=1/2,
       * Polynomials=[109, 79]. This is the Voyager code from NASA:
       * \li   109: b(1101101) --> 1 + x   + x^3 + x^4 + x^6
       * \li   79:  b(1001111) --> 1 + x^3 + x^4 + x^5 + x^6
       *
       * Another encoder class is provided with gr-fec called the
       * gr::fec::code::ccsds_encoder, which implements the above code
       * that is more highly optimized for just those specific
       * settings.
       */
      class FEC_API cc_encoder : virtual public generic_encoder
      {
      public:

        /*!
         * Build a convolutional code encoding FEC API object.
         *
         * \param frame_size Number of bits per frame. If using in the
         *        tagged stream style, this is the maximum allowable
         *        number of bits per frame.
         * \param k Constraint length (K) of the encoder.
         * \param rate Inverse of the coder's rate
         *             (rate=2 means 2 output bits per 1 input).
         * \param polys Vector of polynomials as integers.
         * \param start_state Initialization state of the shift register.
         * \param mode cc_mode_t mode of the encoding.
         * \param padded true if the encoded frame should be padded
         *               to the nearest byte.
         */
        static generic_encoder::sptr make
          (int frame_size, int k, int rate,
           std::vector<int> polys, int start_state = 0,
           cc_mode_t mode=CC_STREAMING, bool padded=false);

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

#endif /* INCLUDED_FEC_CC_ENCODER_H */
