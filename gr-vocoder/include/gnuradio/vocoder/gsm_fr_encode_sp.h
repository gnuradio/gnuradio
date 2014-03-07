/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2013,2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_GSM_FR_ENCODE_SP_H
#define INCLUDED_VOCODER_GSM_FR_ENCODE_SP_H

#include <gnuradio/vocoder/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace vocoder {

    /*!
     * \brief GSM 06.10 Full Rate Vocoder Encoder
     * \ingroup audio_blk
     *
     * Input: 16-bit shorts representing speech samples
     * Output: Vector of 33 bytes per 160 input samples
     */
    class VOCODER_API gsm_fr_encode_sp : virtual public sync_decimator
    {
    public:
      // gr::vocoder::gsm_fr_encode_sp::sptr
      typedef boost::shared_ptr<gsm_fr_encode_sp> sptr;

      /*!
       * \brief Make GSM encoder block.
       */
      static sptr make();
    };

  } /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_GSM_FR_ENCODE_SP_H */
