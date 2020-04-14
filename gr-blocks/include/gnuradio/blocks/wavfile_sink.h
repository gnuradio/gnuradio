/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_WAVFILE_SINK_H
#define INCLUDED_GR_WAVFILE_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Write stream to a Microsoft PCM (.wav) file.
 * \ingroup audio_blk
 *
 * \details
 * Values must be floats within [-1;1].
 * Check gr_make_wavfile_sink() for extra info.
 */
class BLOCKS_API wavfile_sink : virtual public sync_block
{
public:
    // gr::blocks::wavfile_sink::sptr
    typedef std::shared_ptr<wavfile_sink> sptr;

    /*
     * \param filename The .wav file to be opened
     * \param n_channels Number of channels (2 = stereo or I/Q output)
     * \param sample_rate Sample rate [S/s]
     * \param bits_per_sample 16 or 8 bit, default is 16
     */
    static sptr make(const char* filename,
                     int n_channels,
                     unsigned int sample_rate,
                     int bits_per_sample = 16,
                     bool append = false);

    /*!
     * \brief Opens a new file and writes a WAV header. Thread-safe.
     */
    virtual bool open(const char* filename) = 0;

    /*!
     * \brief Closes the currently active file and completes the WAV
     * header. Thread-safe.
     */
    virtual void close() = 0;

    /*!
     * \brief Set the sample rate. This will not affect the WAV file
     * currently opened. Any following open() calls will use this new
     * sample rate.
     */
    virtual void set_sample_rate(unsigned int sample_rate) = 0;

    /*!
     * \brief Set bits per sample. This will not affect the WAV file
     * currently opened (see set_sample_rate()). If the value is
     * neither 8 nor 16, the call is ignored and the current value
     * is kept.
     */
    virtual void set_bits_per_sample(int bits_per_sample) = 0;

    /*!
     * \brief Enable appending to an existing file instead of
     * creating it. This will not affect the WAV file currently
     * opened (see set_sample_rate()).
     */
    virtual void set_append(bool append) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_WAVFILE_SINK_H */
