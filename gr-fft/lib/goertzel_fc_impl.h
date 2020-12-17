/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FFT_GOERTZEL_FC_IMPL_H
#define INCLUDED_FFT_GOERTZEL_FC_IMPL_H

#include <gnuradio/fft/goertzel.h>
#include <gnuradio/fft/goertzel_fc.h>

namespace gr {
namespace fft {

class FFT_API goertzel_fc_impl : public goertzel_fc
{
private:
    goertzel d_goertzel;
    const int d_len;
    float d_freq;
    int d_rate;

public:
    goertzel_fc_impl(int rate, int len, float freq);

    void set_freq(float freq) override;
    void set_rate(int rate) override;

    float freq() const override { return d_freq; }
    int rate() const override { return d_rate; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_GOERTZEL_FC_IMPL_H */
