/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

// This file stores all the RIFF file type knowledge for the wavfile_*
// gnuradio/blocks.

#ifndef _GR_WAVFILE_H_
#define _GR_WAVFILE_H_

#include <cstdio>

namespace gr {
namespace blocks {

//! WAV file header information.
struct wav_header_info {

    //! sample rate [S/s]
    int sample_rate;

    //! Number of channels
    int nchans;

    //! Bytes per sample
    int bytes_per_sample;

    //! Number of samples per channel
    long long samples_per_chan;

    //! sndfile format
    int format;

    //! sndfile format
    int subformat;
};

enum wavfile_format_t {
    FORMAT_WAV = 0x010000,
    FORMAT_FLAC = 0x170000,
    FORMAT_OGG = 0x200000,
    FORMAT_RF64 = 0x220000,
};

enum wavfile_subformat_t {
    FORMAT_PCM_S8 = 1,
    FORMAT_PCM_16,
    FORMAT_PCM_24,
    FORMAT_PCM_32,
    FORMAT_PCM_U8,
    FORMAT_FLOAT,
    FORMAT_DOUBLE,
    FORMAT_VORBIS = 0x0060,
    FORMAT_OPUS = 0x0064,
};

} /* namespace blocks */
} /* namespace gr */

typedef gr::blocks::wavfile_format_t wavfile_format_t;
typedef gr::blocks::wavfile_subformat_t wavfile_subformat_t;

#endif /* _GR_WAVFILE_H_ */
