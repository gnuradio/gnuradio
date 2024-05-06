/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "filter_delay_fc_impl.h"
#include <volk/volk.h>

namespace gr {
namespace filter {

filter_delay_fc::sptr filter_delay_fc::make(const std::vector<float>& taps)
{
    return gnuradio::make_block_sptr<filter_delay_fc_impl>(taps);
}

filter_delay_fc_impl::filter_delay_fc_impl(const std::vector<float>& taps)
    : sync_block("filter_delay_fc",
                 io_signature::make(1, 2, sizeof(float)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_taps(taps),
      d_fir(taps),
      d_delay(d_fir.ntaps() / 2),
      d_update(false)
{
    set_history(d_fir.ntaps());

    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

std::vector<float> filter_delay_fc_impl::taps() { return d_fir.taps(); }

void filter_delay_fc_impl::set_taps(const std::vector<float>& taps)
{
    // don't reset taps in case history changes
    d_taps = taps;
    d_update = true;
}

int filter_delay_fc_impl::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    const float* in0 = (const float*)input_items[0];
    const float* in1;
    gr_complex* out = (gr_complex*)output_items[0];

    if (d_update) {
        d_fir.set_taps(d_taps);
        d_delay = d_fir.ntaps() / 2;
        set_history(d_fir.ntaps());
        return 0;
    }

    switch (input_items.size()) {
    case 1:
        for (int i = 0; i < noutput_items; i++) {
            out[i] = gr_complex(in0[i + d_delay], d_fir.filter(&in0[i]));
        }
        break;

    case 2:
        in1 = (const float*)input_items[1];
        for (int j = 0; j < noutput_items; j++) {
            out[j] = gr_complex(in0[j + d_delay], d_fir.filter(&in1[j]));
        }
        break;

    default:
        assert(0);
    }

    return noutput_items;
}

} /* namespace filter */
} /* namespace gr */
