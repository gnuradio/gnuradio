/* -*- c++ -*- */
/*
 * Copyright 2002,2011,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/fft/goertzel.h>
#include <gnuradio/math.h>
#include <cmath>

namespace gr {
namespace fft {

goertzel::goertzel(int rate, int len, float freq) { set_params(rate, len, freq); }

void goertzel::set_params(int rate, int len, float freq)
{
    d_d1 = 0.0;
    d_d2 = 0.0;

    float w = 2.0 * GR_M_PI * freq / rate;
    d_wr = 2.0 * std::cos(w);
    d_wi = std::sin(w);
    d_len = len;
    d_processed = 0;
}

gr_complex goertzel::batch(const float* in)
{
    d_d1 = 0.0;
    d_d2 = 0.0;

    for (int i = 0; i < d_len; i++)
        input(in[i]);

    return output();
}

void goertzel::input(const float& input)
{
    float y = input + d_wr * d_d1 - d_d2;
    d_d2 = d_d1;
    d_d1 = y;
    d_processed++;
}

gr_complex goertzel::output()
{
    gr_complex out((0.5 * d_wr * d_d1 - d_d2) / d_len, (d_wi * d_d1) / d_len);
    d_d1 = 0.0;
    d_d2 = 0.0;
    d_processed = 0;
    return out;
}

} /* namespace fft */
} /* namespace gr */
