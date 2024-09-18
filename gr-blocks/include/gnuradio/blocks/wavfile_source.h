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
 * Read samples from an audio file (uncompressed or compressed)
 * \ingroup audio_blk
 *
 * \details
 * Unless otherwise called, values are within [-1;1].
 * Accepts audio container from gr-blocks/lib/wavfile_sink_impl.cc
 * .wav files can use PCM-8, PCM-16, PCM-24, PCM-32, float, or double.
 * .flac files can use PCM-S8, PCM-16, or PCM-24.
 * .ogg files can use vorbis or opus.
 * .rf64 files can use PCM-u8, PCM-16, PCM-24, PCM-32, float, or double
 */
class BLOCKS_API wavfile_source : virtual public sync_block
{
public:
    // gr::blocks::wavfile_source::sptr
    typedef std::shared_ptr<wavfile_source> sptr;

    static sptr make(const char* filename, bool repeat = false);

    /*!
     * \brief Read the sample rate as specified in the wav file header
     */
    virtual unsigned int sample_rate() const = 0;

    /*!
     * \brief Return the number of bits per sample as specified in
     * the wav file header. Only 8 or 16 bit are supported here.
     */
    virtual int bits_per_sample() const = 0;

    /*!
     * \brief Return the number of channels in the wav file as
     * specified in the wav file header. This is also the max number
     * of outputs you can have.
     */
    virtual int channels() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_WAVFILE_SOURCE_H */
