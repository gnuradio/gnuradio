/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_WAVFILE_SOURCE_IMPL_H
#define INCLUDED_GR_WAVFILE_SOURCE_IMPL_H

#include <gnuradio/blocks/wavfile_source.h>
#include <cstdio> // for FILE

namespace gr {
namespace blocks {

class wavfile_source_impl : public wavfile_source
{
private:
    FILE* d_fp;
    bool d_repeat;

    unsigned d_sample_rate;
    int d_nchans;
    int d_bytes_per_sample;
    int d_first_sample_pos;
    unsigned d_samples_per_chan;
    unsigned d_sample_idx;
    int d_normalize_shift;
    int d_normalize_fac;

    /*!
     * \brief Convert an integer sample value to a float value within [-1;1]
     */
    float convert_to_float(short int sample);

public:
    wavfile_source_impl(const char* filename, bool repeat);
    ~wavfile_source_impl();

    unsigned int sample_rate() const { return d_sample_rate; };

    int bits_per_sample() const { return d_bytes_per_sample * 8; };

    int channels() const { return d_nchans; };

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_WAVFILE_SOURCE_IMPL_H */
