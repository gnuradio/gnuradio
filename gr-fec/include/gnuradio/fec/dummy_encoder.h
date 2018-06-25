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

#ifndef INCLUDED_FEC_DUMMY_ENCODER_H
#define INCLUDED_FEC_DUMMY_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/encoder.h>
#include <map>
#include <string>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief Dummy Encoding class.
       * \ingroup error_coding_blk
       *
       * \details
       * A dummy encoder class that simply passes the input to the
       * output. It is meant to allow us to easily use the FEC API
       * encoder and decoder blocks in an application with no coding.
       */
      class FEC_API dummy_encoder : virtual public generic_encoder
      {
      public:

        /*!
         * Build a dummy encoding FEC API object.
         *
         * \param frame_size Number of bits per frame. If using in the
         *        tagged stream style, this is the maximum allowable
         *        number of bits per frame.
         * \param pack Determines how to handle the input data stream
         *        as packed or unpacked bits. If set to True, the
         *        encoder's get_input_conversion will be set to
         *        'pack', meaning that the data must be packed before
         *        being sent to this encoder. Generally handled
         *        through an extended encoder interface.
         * \param packed_bits Determines how the output data stream is
         *        handled. If set to True, the encoder's
         *        get_output_conversion returns 'packed_bits', meaning
         *        that the output data of the encoder is packed and
         *        should be unpacked. Generally handled through an
         *        extended encoder interface.
         */
        static generic_encoder::sptr make(int frame_size, bool pack = false,
                                          bool packed_bits = false);

        /*!
         * Sets the uncoded frame size to \p frame_size. If \p
         * frame_size is greater than the value given to the
         * constructor, the frame size will be capped by that initial
         * value and this function will return false. Otherwise, it
         * returns true.
         */
        virtual bool set_frame_size(unsigned int frame_size) = 0;

        /*!
         * Returns the coding rate of this encoder (it will always be 1).
         */
        virtual double rate() = 0;
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DUMMY_ENCODER_H */
