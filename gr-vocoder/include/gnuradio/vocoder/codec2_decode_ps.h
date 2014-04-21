/* -*- c++ -*- */
/*
 * Copyright 2011,2013,2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_CODEC2_DECODE_PS_H
#define INCLUDED_VOCODER_CODEC2_DECODE_PS_H

#include <gnuradio/vocoder/api.h>
#include <gnuradio/vocoder/codec2.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace vocoder {

    /*!
     * \brief CODEC2 Vocoder Decoder
     * \ingroup audio_blk
     *
     * Input: A vector of unpacked bits forming a Codec2 frame.
     *
     * Output: 16-bit short values of an audio signal with sampling rate 8 kHz.
     *
     * See also gr::vocoder::codec2_encode_sp.
     */
    class VOCODER_API codec2_decode_ps : virtual public sync_interpolator
    {
    public:
      // gr::vocoder::codec2_decode_ps::sptr
      typedef boost::shared_ptr<codec2_decode_ps> sptr;

      /*!
       * \brief Make Codec2 decoder block.
       *
       * \param mode Encoded bit rate/mode
       */
      static sptr make(int mode=codec2::MODE_2400);
    };

  } /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_CODEC2_DECODE_PS_H */
