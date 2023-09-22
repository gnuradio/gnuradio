/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fft_filter_ccc_impl.h"
#include <gnuradio/io_signature.h>

#include <cmath>
#include <stdexcept>

namespace gr {
namespace filter {

fft_filter_ccc::sptr
fft_filter_ccc::make(int decimation, const std::vector<gr_complex>& taps, int nthreads)
{
    return gnuradio::make_block_sptr<fft_filter_ccc_impl>(decimation, taps, nthreads);
}

fft_filter_ccc_impl::fft_filter_ccc_impl(int decimation,
                                         const std::vector<gr_complex>& taps,
                                         int nthreads)
    : sync_decimator("fft_filter_ccc",
                     io_signature::make(1, 1, sizeof(gr_complex)),
                     io_signature::make(1, 1, sizeof(gr_complex)),
                     decimation),
      d_updated(false),
      d_filter(decimation, taps, nthreads)
{
    d_new_taps = taps;
    d_nsamples = d_filter.set_taps(taps);
    set_output_multiple(d_nsamples);
}

void fft_filter_ccc_impl::set_taps(const std::vector<gr_complex>& taps)
{
    d_new_taps = taps;
    d_updated = true;
}

std::vector<gr_complex> fft_filter_ccc_impl::taps() const { return d_new_taps; }

void fft_filter_ccc_impl::set_nthreads(int n) { d_filter.set_nthreads(n); }

int fft_filter_ccc_impl::nthreads() const { return d_filter.nthreads(); }

int fft_filter_ccc_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    if (d_updated) {
        d_nsamples = d_filter.set_taps(d_new_taps);
        d_updated = false;
        set_output_multiple(d_nsamples);
        return 0; // output multiple may have changed
    }

    d_filter.filter(noutput_items, in, out);

    return noutput_items;
}

} /* namespace filter */
} /* namespace gr */
