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

#include "pfb_interpolator_ccf_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace filter {

pfb_interpolator_ccf::sptr pfb_interpolator_ccf::make(unsigned int interp,
                                                      const std::vector<float>& taps)
{
    return gnuradio::make_block_sptr<pfb_interpolator_ccf_impl>(interp, taps);
}


pfb_interpolator_ccf_impl::pfb_interpolator_ccf_impl(unsigned int interp,
                                                     const std::vector<float>& taps)
    : sync_interpolator("pfb_interpolator_ccf",
                        io_signature::make(1, 1, sizeof(gr_complex)),
                        io_signature::make(1, 1, sizeof(gr_complex)),
                        interp),
      polyphase_filterbank(interp, taps),
      d_updated(false),
      d_rate(interp)
{
    set_history(d_taps_per_filter);
}

void pfb_interpolator_ccf_impl::set_taps(const std::vector<float>& taps)
{
    gr::thread::scoped_lock guard(d_mutex);

    polyphase_filterbank::set_taps(taps);
    set_history(d_taps_per_filter);
    d_updated = true;
}

void pfb_interpolator_ccf_impl::print_taps() { polyphase_filterbank::print_taps(); }

std::vector<std::vector<float>> pfb_interpolator_ccf_impl::taps() const
{
    return polyphase_filterbank::taps();
}

int pfb_interpolator_ccf_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    if (d_updated) {
        d_updated = false;
        return 0; // history requirements may have changed.
    }

    int i = 0, count = 0;

    while (i < noutput_items) {
        for (unsigned int j = 0; j < d_rate; j++) {
            out[i] = d_fir_filters[j].filter(&in[count]);
            i++;
        }
        count++;
    }

    return i;
}

} /* namespace filter */
} /* namespace gr */
