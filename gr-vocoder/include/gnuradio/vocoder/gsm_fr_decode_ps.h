/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_GSM_FR_DECODE_PS_H
#define INCLUDED_VOCODER_GSM_FR_DECODE_PS_H

#include <gnuradio/vocoder/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace vocoder {

    /*!
     * \brief GSM 06.10 Full Rate Vocoder Decoder
     * \ingroup audio_blk
     *
     * Input: Vector of 33 bytes per 160 input samples
     * Output: 16-bit shorts representing speech samples
     */
    class VOCODER_API gsm_fr_decode_ps : virtual public sync_interpolator
    {
    public:
      // gr::vocoder::gsm_fr_decode_ps::sptr
      typedef boost::shared_ptr<gsm_fr_decode_ps> sptr;

      /*!
       * \brief Make GSM decoder block.
       */
      static sptr make();
    };

  } /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_GSM_FR_DECODE_PS_H */
