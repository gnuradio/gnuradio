/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 3, or (at your
 * option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_H
#define INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/fec_mtrx.h>
#include <gnuradio/fec/generic_decoder.h>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief LDPC bit flip decoding class
       * \ingroup error_coding_blk
       *
       * \details
       * A hard decision bit flip decoder class for decoding low
       * density parity check (LDPC) codes. The decoder requires
       * knowledge of the matrix used to create (encode) the
       * codewords.  The simple algorithm is:
       *
       * 1. Compute parity checks on all of the bits.
       * 2. Flip the bit(s) associated with the most failed parity
       *    checks.
       * 3. Check to see if new word is valid. (\f$\overline{x}\f$
       *    is a codeword if and only if
       *    \f$\mathbf{H}\bar{x}^{T}=\overline{0}\f$.) If it is not
       *    valid, go back to step 1.
       * 4. Repeat until valid codeword is found or the maximum
       *    number of iterations is reached.
       */
      class FEC_API ldpc_bit_flip_decoder : virtual public generic_decoder
      {
      public:

        /*!
         * \brief Build a bit flip decoding FEC API object.
         * \param mtrx_obj The LDPC parity check matrix to use for
         *        decoding. This should be the same matrix used for
         *        encoding. Provide either a ldpc_H_matrix or
         *        a ldpc_G_matrix object.
         * \param max_iter Maximum number of iterations to complete
         *        during the decoding algorithm. The default is 100
         *        because this seemed to be sufficient during
         *        testing. May be increased for possibly better
         *        performance, but may slow things down.
         */
        static generic_decoder::sptr make(const fec_mtrx_sptr mtrx_obj,
                                          unsigned int max_iter=100);

        /*!
         * \brief  Sets the uncoded frame size to \p frame_size.
         * \details
         * Sets the uncoded frame size to \p frame_size. If \p
         * frame_size is greater than the value given to the
         * constructor, the frame size will be capped by that initial
         * value and this function will return false. Otherwise, it
         * returns true.
         */
        virtual bool set_frame_size(unsigned int frame_size) = 0;

        //! Returns the coding rate of this decoder.
        virtual double rate() = 0;

      };
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_H */
