/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_FILTER_FFT_FILTER_FFF_IMPL_H
#define INCLUDED_FILTER_FFT_FILTER_FFF_IMPL_H

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fft_filter.h>
#include <gnuradio/filter/fft_filter_fff.h>

namespace gr {
namespace filter {

class FILTER_API fft_filter_fff_impl : public fft_filter_fff
{
private:
    int d_nsamples;
    bool d_updated;
    kernel::fft_filter_fff d_filter;
    std::vector<float> d_new_taps;

public:
    fft_filter_fff_impl(int decimation, const std::vector<float>& taps, int nthreads = 1);

    void set_taps(const std::vector<float>& taps);
    std::vector<float> taps() const;

    void set_nthreads(int n);
    int nthreads() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FFT_FILTER_FFF_IMPL_H */
