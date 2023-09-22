/* -*- c++ -*- */
/*
 * Copyright 2011.2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_ALAW_ENCODER_SB_H
#define INCLUDED_VOCODER_ALAW_ENCODER_SB_H

#include <gnuradio/sync_block.h>
#include <gnuradio/vocoder/api.h>

namespace gr {
namespace vocoder {

/*!
 * \brief This block performs g.711 alaw audio encoding.
 * \ingroup audio_blk
 */
class VOCODER_API alaw_encode_sb : virtual public sync_block
{
public:
    // gr::vocoder::alaw_encode_sb::sptr
    typedef std::shared_ptr<alaw_encode_sb> sptr;

    /*!
     * \brief Make alaw encoder block.
     */
    static sptr make();
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_ALAW_ENCODE_SB_H */
