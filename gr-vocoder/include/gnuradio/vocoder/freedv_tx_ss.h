/* -*- c++ -*- */
/*
 * Copyright 2016-2019 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_FREEDV_TX_H
#define INCLUDED_VOCODER_FREEDV_TX_H

#include <gnuradio/sync_block.h>
#include <gnuradio/vocoder/api.h>
#include <gnuradio/vocoder/freedv_api.h>

namespace gr {
namespace vocoder {

/*!
 * \brief FreeDV modulator
 * \ingroup audio_blk
 *
 * Input: Speech (audio) signal as 16-bit shorts, sampling rate 8 kHz.
 *
 * Output: Signal (audio) as 16-bit shorts, sampling rate 8 kHz.
 *
 */
class VOCODER_API freedv_tx_ss : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<freedv_tx_ss> sptr;

    /*!
     * \brief Make FreeDV Modem modulator block.
     *
     * \param mode Operating Mode designation
     * \param msg_txt Low Rate message text (callsign, location)
     * \param interleave_frames FreeDV 700D mode number of frames to average error
     */
    static sptr make(int mode = freedv_api::MODE_1600,
                     const std::string msg_txt = "GNU Radio",
                     int interleave_frames = 1);
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_FREEDV_TX_H */
