/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_FILTER_FFT_FILTER_CCC_IMPL_H
#define INCLUDED_FILTER_FFT_FILTER_CCC_IMPL_H

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fft_filter.h>
#include <gnuradio/filter/fft_filter_ccc.h>

namespace gr {
namespace filter {

class FILTER_API fft_filter_ccc_impl : public fft_filter_ccc
{
private:
    int d_nsamples;
    bool d_updated;
    kernel::fft_filter_ccc d_filter;
    std::vector<gr_complex> d_new_taps;

public:
    fft_filter_ccc_impl(int decimation,
                        const std::vector<gr_complex>& taps,
                        int nthreads = 1);

    void set_taps(const std::vector<gr_complex>& taps) override;
    std::vector<gr_complex> taps() const override;

    void set_nthreads(int n) override;
    int nthreads() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FFT_FILTER_CCC_IMPL_H */
