/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "filterbank_vcvcf_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>

namespace gr {
namespace filter {

filterbank_vcvcf::sptr filterbank_vcvcf::make(const std::vector<std::vector<float>>& taps)
{
    return gnuradio::make_block_sptr<filterbank_vcvcf_impl>(taps);
}

filterbank_vcvcf_impl::filterbank_vcvcf_impl(const std::vector<std::vector<float>>& taps)
    : block("filterbank_vcvcf",
            io_signature::make(1, 1, sizeof(gr_complex) * taps.size()),
            io_signature::make(1, 1, sizeof(gr_complex) * taps.size())),
      filterbank(taps),
      d_updated(false)
{
    set_history(d_ntaps);
}

void filterbank_vcvcf_impl::set_taps(const std::vector<std::vector<float>>& taps)
{
    gr::thread::scoped_lock guard(d_mutex);
    filterbank::set_taps(taps);
    set_history(d_ntaps);
    d_updated = true;
}

void filterbank_vcvcf_impl::print_taps() { filterbank::print_taps(); }

std::vector<std::vector<float>> filterbank_vcvcf_impl::taps() const
{
    return filterbank::taps();
}

int filterbank_vcvcf_impl::general_work(int noutput_items,
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

    volk::vector<gr_complex> working(noutput_items + d_ntaps - 1);

    for (unsigned int i = 0; i < d_nfilts; i++) {
        // Only call the filter method on active filters.
        if (d_active[i]) {
            for (unsigned int j = 0; j < noutput_items + d_ntaps - 1; j++) {
                unsigned int p = i + j * d_nfilts;
                working[j] = in[p];
            }
            for (unsigned int j = 0; j < (unsigned int)(noutput_items); j++) {
                unsigned int p = i + j * d_nfilts;
                out[p] = d_fir_filters[i].filter(&working[j]);
            }
        } else {
            // Otherwise just output 0s.
            for (unsigned int j = 0; j < (unsigned int)(noutput_items); j++) {
                unsigned int p = i + j * d_nfilts;
                out[p] = 0;
            }
        }
    }

    consume_each(noutput_items);
    return noutput_items;
}

} /* namespace filter */
} /* namespace gr */
