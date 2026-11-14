/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pfb_arb_resampler_ccc_impl.h"
#include "resampler_tags.h"
#include <gnuradio/io_signature.h>
#include <algorithm>
#include <cstdio>

namespace gr {
namespace filter {

pfb_arb_resampler_ccc::sptr pfb_arb_resampler_ccc::make(
    float rate, const std::vector<gr_complex>& taps, unsigned int filter_size)
{
    return gnuradio::make_block_sptr<pfb_arb_resampler_ccc_impl>(rate, taps, filter_size);
}


pfb_arb_resampler_ccc_impl::pfb_arb_resampler_ccc_impl(
    float rate, const std::vector<gr_complex>& taps, unsigned int filter_size)
    : block("pfb_arb_resampler_ccc",
            io_signature::make(1, 1, sizeof(gr_complex)),
            io_signature::make(1, 1, sizeof(gr_complex))),
      d_resamp(rate, taps, filter_size)
{
    d_updated = false;

    set_history(d_resamp.taps_per_filter());
    set_relative_rate(rate);
    set_tag_propagation_policy(TPP_DONT); // tags are placed in general_work()
    if (rate >= 1.0f) {
        unsigned output_multiple = std::max<int>(rate, filter_size);
        set_output_multiple(output_multiple);
    }
}

void pfb_arb_resampler_ccc_impl::forecast(int noutput_items,
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

void pfb_arb_resampler_ccc_impl::set_taps(const std::vector<gr_complex>& taps)
{
    gr::thread::scoped_lock guard(d_mutex);

    d_resamp.set_taps(taps);
    set_history(d_resamp.taps_per_filter());
    d_updated = true;
}

std::vector<std::vector<gr_complex>> pfb_arb_resampler_ccc_impl::taps() const
{
    return d_resamp.taps();
}

void pfb_arb_resampler_ccc_impl::print_taps() { d_resamp.print_taps(); }

void pfb_arb_resampler_ccc_impl::set_rate(float rate)
{
    gr::thread::scoped_lock guard(d_mutex);

    d_resamp.set_rate(rate);
    set_relative_rate(rate);
    d_rate_in = nitems_read(0);
    d_rate_out = nitems_written(0);
}

void pfb_arb_resampler_ccc_impl::set_phase(float ph)
{
    gr::thread::scoped_lock guard(d_mutex);
    d_resamp.set_phase(ph);
}

float pfb_arb_resampler_ccc_impl::phase() const { return d_resamp.phase(); }

unsigned int pfb_arb_resampler_ccc_impl::interpolation_rate() const
{
    return d_resamp.interpolation_rate();
}

unsigned int pfb_arb_resampler_ccc_impl::decimation_rate() const
{
    return d_resamp.decimation_rate();
}

float pfb_arb_resampler_ccc_impl::fractional_rate() const
{
    return d_resamp.fractional_rate();
}

unsigned int pfb_arb_resampler_ccc_impl::taps_per_filter() const
{
    return d_resamp.taps_per_filter();
}

int pfb_arb_resampler_ccc_impl::group_delay() const { return d_resamp.group_delay(); }

float pfb_arb_resampler_ccc_impl::phase_offset(float freq, float fs)
{
    return d_resamp.phase_offset(freq, fs);
}

int pfb_arb_resampler_ccc_impl::general_work(int noutput_items,
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

    // Place tags like the default propagation does, but also as soon as the
    // output a tag maps to has been produced: this block can produce that
    // output before it consumes the tagged sample, and a tag added only on
    // consumption could land on output that was already read downstream.
    const uint64_t consumed_end = this->nitems_read(0) + nitems_read;
    const uint64_t written_end = nitems_written(0) + processed;
    std::vector<tag_t> tags;
    get_tags_in_range(tags, 0, d_tag_next, this->nitems_read(0) + ninput_items[0]);
    for (auto& tag : tags) {
        const uint64_t in_offset = tag.offset;
        tag.offset =
            resampler_tag_offset(in_offset, d_rate_in, d_rate_out, mp_relative_rate());
        if (in_offset >= consumed_end && tag.offset >= written_end) {
            break;
        }
        add_item_tag(0, tag);
        d_tag_next = in_offset + 1;
    }
    d_tag_next = std::max(d_tag_next, consumed_end);

    consume_each(nitems_read);
    return processed;
}

} /* namespace filter */
} /* namespace gr */
