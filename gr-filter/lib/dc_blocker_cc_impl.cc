/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dc_blocker_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <memory>

namespace gr {
namespace filter {

moving_averager_c::moving_averager_c(int D)
    : d_length(D),
      d_out(0),
      d_out_d1(0),
      d_out_d2(0),
      d_delay_line(d_length - 1, gr_complex(0, 0))
{
}

gr_complex moving_averager_c::filter(gr_complex x)
{
    d_out_d1 = d_out;
    d_delay_line.push_back(x);
    d_out = d_delay_line[0];
    d_delay_line.pop_front();

    gr_complex y = x - d_out_d1 + d_out_d2;
    d_out_d2 = y;

    return (y / (float)(d_length));
}


dc_blocker_cc::sptr dc_blocker_cc::make(int D, bool long_form)
{
    return gnuradio::make_block_sptr<dc_blocker_cc_impl>(D, long_form);
}


dc_blocker_cc_impl::dc_blocker_cc_impl(int D, bool long_form)
    : sync_block("dc_blocker_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_length(D),
      d_long_form(long_form),
      d_ma_0(D),
      d_ma_1(D)
{
    if (d_long_form) {
        d_ma_2 = std::make_unique<moving_averager_c>(D);
        d_ma_3 = std::make_unique<moving_averager_c>(D);
        d_delay_line = std::deque<gr_complex>(d_length - 1, gr_complex(0, 0));
    }
}

int dc_blocker_cc_impl::group_delay()
{
    if (d_long_form)
        return (2 * d_length - 2);
    else
        return d_length - 1;
}

int dc_blocker_cc_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    if (d_long_form) {
        gr_complex y1, y2, y3, y4, d;
        for (int i = 0; i < noutput_items; i++) {
            y1 = d_ma_0.filter(in[i]);
            y2 = d_ma_1.filter(y1);
            y3 = d_ma_2->filter(y2);
            y4 = d_ma_3->filter(y3);

            d_delay_line.push_back(d_ma_0.delayed_sig());
            d = d_delay_line[0];
            d_delay_line.pop_front();

            out[i] = d - y4;
        }
    } else {
        gr_complex y1, y2;
        for (int i = 0; i < noutput_items; i++) {
            y1 = d_ma_0.filter(in[i]);
            y2 = d_ma_1.filter(y1);
            out[i] = d_ma_0.delayed_sig() - y2;
        }
    }

    return noutput_items;
}

} /* namespace filter */
} /* namespace gr */
