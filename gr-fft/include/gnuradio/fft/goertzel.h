/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FFT_IMPL_GOERTZEL_H
#define INCLUDED_FFT_IMPL_GOERTZEL_H

#include <gnuradio/fft/api.h>
#include <gnuradio/types.h>

namespace gr {
namespace fft {

/*!
 * \brief Implements Goertzel single-bin DFT calculation
 * \ingroup misc
 */
class FFT_API goertzel
{
public:
    goertzel(int rate, int len, float freq);

    void set_params(int rate, int len, float freq);

    // Process a input array
    gr_complex batch(const float* in);

    // Process sample by sample
    void input(const float& in);
    gr_complex output();
    bool ready() const { return d_processed == d_len; }

private:
    float d_d1;
    float d_d2;
    float d_wr;
    float d_wi;
    int d_len;
    int d_processed;
};

} /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_IMPL_GOERTZEL_H */
