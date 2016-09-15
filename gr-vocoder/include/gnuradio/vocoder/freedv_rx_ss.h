/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_FREEDV_RX_H
#define INCLUDED_VOCODER_FREEDV_RX_H

#include <gnuradio/vocoder/api.h>
#include <gnuradio/vocoder/freedv_api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace vocoder {

    /*!
     * \brief FreeDV demodulator
     * \ingroup audio_blk
     *
     * Input: 16-bit short values of an audio signal with sampling rate 8 kHz.
     *
     * Output: 16-bit short values of an audio signal with sampling rate 8 kHz.
     *
     * See also gr::vocoder::freedv_tx_ss.
     */
    class VOCODER_API freedv_rx_ss : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<freedv_rx_ss> sptr;

      /*!
       * \brief Make FreeDV modem demodulator block.
       *
       * \param mode Operating Mode designation
       * \param squelch_thresh FreeDV modem squelch threshold value
       */
      static sptr make(int mode=freedv_api::MODE_1600, float squelch_thresh=-100.0);

      virtual void set_squelch_thresh(float squelch_thresh) = 0;
      virtual float squelch_thresh() = 0;
    };

  } /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_FREEDV_RX_H */
