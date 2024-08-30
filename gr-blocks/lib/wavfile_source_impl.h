/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2013,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_WAVFILE_SOURCE_IMPL_H
#define INCLUDED_GR_WAVFILE_SOURCE_IMPL_H

#include <gnuradio/blocks/wavfile.h>
#include <gnuradio/blocks/wavfile_source.h>
#include <sndfile.h> // for SNDFILE

namespace gr {
namespace blocks {

class wavfile_source_impl : public wavfile_source
{
private:
    SNDFILE* d_fp;
    bool d_repeat;

    wav_header_info d_h;
    std::vector<float> d_buffer;

    static constexpr int s_items_size = 1024;
    static constexpr int s_max_channels = 24;

public:
    wavfile_source_impl(const char* filename, bool repeat);
    ~wavfile_source_impl() override;

    unsigned int sample_rate() const override { return d_h.sample_rate; };

    int bits_per_sample() const override { return d_h.bytes_per_sample * 8; };

    int channels() const override { return d_h.nchans; };

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_WAVFILE_SOURCE_IMPL_H */
