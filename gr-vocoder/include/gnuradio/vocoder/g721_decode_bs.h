/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_G721_DECODE_BS_H
#define INCLUDED_VOCODER_G721_DECODE_BS_H

#include <gnuradio/sync_block.h>
#include <gnuradio/vocoder/api.h>

namespace gr {
namespace vocoder {

/*!
 * \brief This block performs g721 audio decoding.
 * \ingroup audio_blk
 */
class VOCODER_API g721_decode_bs : virtual public sync_block
{
public:
    // gr::vocoder::g721_decode_bs::sptr
    typedef std::shared_ptr<g721_decode_bs> sptr;

    /*!
     * \brief Make G721 decoder block.
     */
    static sptr make();
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_G721_DECODE_BS_H */
