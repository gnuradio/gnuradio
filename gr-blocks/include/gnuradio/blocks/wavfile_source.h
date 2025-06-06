/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_WAVFILE_SOURCE_H
#define INCLUDED_GR_WAVFILE_SOURCE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Read samples from an audio file (uncompressed or compressed)
 * \ingroup audio_blk
 *
 * \details
 * Unless otherwise called, values are within [-1;1].
 * Check gr_make_wavfile_source() for extra info.
 */
class BLOCKS_API wavfile_source : virtual public sync_block
{
public:
    // gr::blocks::wavfile_source::sptr
    typedef std::shared_ptr<wavfile_source> sptr;

    static sptr make(const char* filename, bool repeat = false);

    /*!
     * \brief Read the sample rate as specified in the audio file metadata
     */
    virtual unsigned int sample_rate() const = 0;

    /*!
     * \brief Return the number of bits per sample as specified in
     * the audio file metadata. Only 8 or 16 bit are supported here.
     */
    virtual int bits_per_sample() const = 0;

    /*!
     * \brief Return the number of channels in the audio file as
     * specified in the audio file metadata. This is also the max number
     * of outputs you can have.
     */
    virtual int channels() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_WAVFILE_SOURCE_H */
