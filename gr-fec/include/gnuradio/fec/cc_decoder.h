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
       * The decoder is set up with a number of bits per frame in the
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
       * continually encoded. This mode is the only mode for this
       * decoder that has a history requirement because it requires
       * rate*(K-1) bits more to finish the decoding properly. This
       * mode does not work with any deployments that do not allow
       * history.
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
       */
      class FEC_API cc_decoder : virtual public generic_decoder
      {
      public:

        /*!
         * Build a convolutional code decoding FEC API object.
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
