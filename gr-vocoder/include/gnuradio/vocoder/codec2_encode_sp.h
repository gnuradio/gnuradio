/* -*- c++ -*- */
/*
 * Copyright 2005,2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_CODEC2_ENCODE_SP_H
#define INCLUDED_VOCODER_CODEC2_ENCODE_SP_H

#include <gnuradio/sync_decimator.h>
#include <gnuradio/vocoder/api.h>
#include <gnuradio/vocoder/codec2.h>

namespace gr {
namespace vocoder {

/*!
 * \brief CODEC2 Vocoder Encoder
 * \ingroup audio_blk
 *
 * Input: Speech (audio) signal as 16-bit shorts, sampling rate 8 kHz.
 *
 * Output: Vector of unpacked bits, forming one Codec2 frame, per 160
 *         input samples (in 2400 and 3200 bps modes) or per 320 input
 *         samples (in 1200, 1300, 1400 and 1600 bps modes).
 *
 */
class VOCODER_API codec2_encode_sp : virtual public sync_decimator
{
public:
    // gr::vocoder::codec2_encode_sp::sptr
    typedef std::shared_ptr<codec2_encode_sp> sptr;

    /*!
     * \brief Make Codec2 encoder block.
     *
     * \param mode Encoded bit rate/mode
     */
    static sptr make(int mode = codec2::MODE_2400);
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_CODEC2_ENCODE_SP_H */
