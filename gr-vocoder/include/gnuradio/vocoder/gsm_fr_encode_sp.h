/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_GSM_FR_ENCODE_SP_H
#define INCLUDED_VOCODER_GSM_FR_ENCODE_SP_H

#include <gnuradio/sync_decimator.h>
#include <gnuradio/vocoder/api.h>

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
    typedef std::shared_ptr<gsm_fr_encode_sp> sptr;

    /*!
     * \brief Make GSM encoder block.
     */
    static sptr make();
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_GSM_FR_ENCODE_SP_H */
