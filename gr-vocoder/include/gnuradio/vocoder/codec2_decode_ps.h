/* -*- c++ -*- */
/*
 * Copyright 2011,2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_CODEC2_DECODE_PS_H
#define INCLUDED_VOCODER_CODEC2_DECODE_PS_H

#include <gnuradio/sync_interpolator.h>
#include <gnuradio/vocoder/api.h>
#include <gnuradio/vocoder/codec2.h>

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
    typedef std::shared_ptr<codec2_decode_ps> sptr;

    /*!
     * \brief Make Codec2 decoder block.
     *
     * \param mode Encoded bit rate/mode
     */
    static sptr make(int mode = codec2::MODE_2400);
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_CODEC2_DECODE_PS_H */
