/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pfb_arb_resampler_ccf_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>

namespace gr {
namespace filter {

pfb_arb_resampler_ccf::sptr pfb_arb_resampler_ccf::make(float rate,
                                                        const std::vector<float>& taps,
                                                        unsigned int filter_size)
{
    return gnuradio::make_block_sptr<pfb_arb_resampler_ccf_impl>(rate, taps, filter_size);
}


pfb_arb_resampler_ccf_impl::pfb_arb_resampler_ccf_impl(float rate,
                                                       const std::vector<float>& taps,
                                                       unsigned int filter_size)
    : block("pfb_arb_resampler_ccf",
            io_signature::make(1, 1, sizeof(gr_complex)),
            io_signature::make(1, 1, sizeof(gr_complex))),
      d_resamp(rate, taps, filter_size)
{
    d_updated = false;

    set_history(d_resamp.taps_per_filter());
    set_relative_rate(rate);
    if (rate >= 1.0f) {
        unsigned output_multiple = std::max<int>(rate, filter_size);
        set_output_multiple(output_multiple);
    }
}

void pfb_arb_resampler_ccf_impl::forecast(int noutput_items,
                                          gr_vector_int& ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    if (noutput_items / relative_rate() < 1) {
        for (unsigned i = 0; i < ninputs; i++)
            ninput_items_required[i] = relative_rate() + history() - 1;
    } else {
        for (unsigned i = 0; i < ninputs; i++)
            ninput_items_required[i] = noutput_items / relative_rate() + history() - 1;
    }
}

void pfb_arb_resampler_ccf_impl::set_taps(const std::vector<float>& taps)
{
    gr::thread::scoped_lock guard(d_mutex);

    d_resamp.set_taps(taps);
    set_history(d_resamp.taps_per_filter());
    d_updated = true;
}

std::vector<std::vector<float>> pfb_arb_resampler_ccf_impl::taps() const
{
    return d_resamp.taps();
}

void pfb_arb_resampler_ccf_impl::print_taps() { d_resamp.print_taps(); }

void pfb_arb_resampler_ccf_impl::set_rate(float rate)
{
    gr::thread::scoped_lock guard(d_mutex);

    d_resamp.set_rate(rate);
    set_relative_rate(rate);
}

void pfb_arb_resampler_ccf_impl::set_phase(float ph)
{
    gr::thread::scoped_lock guard(d_mutex);
    d_resamp.set_phase(ph);
}

float pfb_arb_resampler_ccf_impl::phase() const { return d_resamp.phase(); }

unsigned int pfb_arb_resampler_ccf_impl::interpolation_rate() const
{
    return d_resamp.interpolation_rate();
}

unsigned int pfb_arb_resampler_ccf_impl::decimation_rate() const
{
    return d_resamp.decimation_rate();
}

float pfb_arb_resampler_ccf_impl::fractional_rate() const
{
    return d_resamp.fractional_rate();
}

unsigned int pfb_arb_resampler_ccf_impl::taps_per_filter() const
{
    return d_resamp.taps_per_filter();
}

int pfb_arb_resampler_ccf_impl::group_delay() const { return d_resamp.group_delay(); }

float pfb_arb_resampler_ccf_impl::phase_offset(float freq, float fs)
{
    return d_resamp.phase_offset(freq, fs);
}

int pfb_arb_resampler_ccf_impl::general_work(int noutput_items,
                                             gr_vector_int& ninput_items,
                                             gr_vector_const_void_star& input_items,
                                             gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_mutex);

    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    if (d_updated) {
        d_updated = false;
        return 0; // history requirements may have changed.
    }

    int nitems_read;
    int nitems = floorf((float)noutput_items / relative_rate());
    int processed = d_resamp.filter(out, in, nitems, nitems_read);

    consume_each(nitems_read);
    return processed;
}

} /* namespace filter */
} /* namespace gr */
