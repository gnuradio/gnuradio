/* -*- c++ -*- */
/*
 * Copyright 2016-2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_FREEDV_RX_H
#define INCLUDED_VOCODER_FREEDV_RX_H

#include <gnuradio/block.h>
#include <gnuradio/vocoder/api.h>
#include <gnuradio/vocoder/freedv_api.h>

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
    typedef std::shared_ptr<freedv_rx_ss> sptr;

    /*!
     * \brief Make FreeDV modem demodulator block.
     *
     * \param mode Operating Mode designation
     * \param squelch_thresh FreeDV modem squelch threshold value
     * \param interleave_frames FreeDV 700D mode number of frames to average error
     */
    static sptr make(int mode = freedv_api::MODE_1600,
                     float squelch_thresh = -100.0,
                     int interleave_frames = 1);

    virtual void set_squelch_thresh(float squelch_thresh) = 0;
    virtual float squelch_thresh() = 0;
    virtual void set_squelch_en(bool squelch_enable) = 0;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_FREEDV_RX_H */
