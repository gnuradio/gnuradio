/* -*- c++ -*- */
/*
 * Copyright 2016-2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
 * Input: Speech (audio) signal as 16-bit shorts.
 *
 * Output: Signal (audio) as 16-bit shorts.
 *
 */
class VOCODER_API freedv_tx_ss : virtual public gr::block
{
public:
    typedef std::shared_ptr<freedv_tx_ss> sptr;

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
