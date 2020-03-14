/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_GSM_FR_DECODE_PS_H
#define INCLUDED_VOCODER_GSM_FR_DECODE_PS_H

#include <gnuradio/sync_interpolator.h>
#include <gnuradio/vocoder/api.h>

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
    typedef std::shared_ptr<gsm_fr_decode_ps> sptr;

    /*!
     * \brief Make GSM decoder block.
     */
    static sptr make();
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_GSM_FR_DECODE_PS_H */
