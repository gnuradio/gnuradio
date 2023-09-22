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

#include "dc_blocker_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <memory>

namespace gr {
namespace filter {

moving_averager_f::moving_averager_f(int D)
    : d_length(D), d_out(0), d_out_d1(0), d_out_d2(0), d_delay_line(d_length - 1, 0)
{
}

float moving_averager_f::filter(float x)
{
    d_out_d1 = d_out;
    d_delay_line.push_back(x);
    d_out = d_delay_line[0];
    d_delay_line.pop_front();

    float y = x - d_out_d1 + d_out_d2;
    d_out_d2 = y;

    return (y / (float)(d_length));
}


dc_blocker_ff::sptr dc_blocker_ff::make(int D, bool long_form)
{
    return gnuradio::make_block_sptr<dc_blocker_ff_impl>(D, long_form);
}

dc_blocker_ff_impl::dc_blocker_ff_impl(int D, bool long_form)
    : sync_block("dc_blocker_ff",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float))),
      d_length(D),
      d_long_form(long_form),
      d_ma_0(D),
      d_ma_1(D)
{
    if (d_long_form) {
        d_ma_2 = std::make_unique<moving_averager_f>(D);
        d_ma_3 = std::make_unique<moving_averager_f>(D);
        d_delay_line = std::deque<float>(d_length - 1, 0);
    }
}

int dc_blocker_ff_impl::group_delay()
{
    if (d_long_form)
        return (2 * d_length - 2);
    else
        return d_length - 1;
}

int dc_blocker_ff_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];

    if (d_long_form) {
        float y1, y2, y3, y4, d;
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
        float y1, y2;
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
