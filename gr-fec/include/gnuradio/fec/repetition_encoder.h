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

#ifndef INCLUDED_FEC_REPETITION_ENCODER_H
#define INCLUDED_FEC_REPETITION_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/encoder.h>
#include <map>
#include <string>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief Repetition Encoding class.
       * \ingroup error_coding_blk
       *
       * \details
       * A repetition encoder class that repeats each input bit \p rep
       * times. To  decode, take  a majority vote  over the  number of
       * repetitions.
       */
      class FEC_API repetition_encoder : virtual public generic_encoder
      {
      public:

        /*!
         * Build a repetition encoding FEC API object.
         *
         * \param frame_size Number of bits per frame. If using in the
         *        tagged stream style, this is the maximum allowable
         *        number of bits per frame.
         * \param rep Repetition rate; encoder rate is rep bits out
         *        for each input bit.
         */
        static generic_encoder::sptr make(int frame_size, int rep);

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

#endif /* INCLUDED_FEC_REPETITION_ENCODER_H */
