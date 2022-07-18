/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "dc_blocker_cpu.h"
#include "dc_blocker_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace filter {

template <class T>
dc_blocker_cpu<T>::dc_blocker_cpu(const typename dc_blocker<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T),
      d_length(args.D),
      d_long_form(args.long_form),
      d_ma_0(args.D),
      d_ma_1(args.D)
{
    if (d_long_form) {
        d_ma_2 = std::make_unique<kernel::filter::moving_averager<T>>(d_length);
        d_ma_3 = std::make_unique<kernel::filter::moving_averager<T>>(d_length);
        d_delay_line = std::deque<T>(d_length - 1, 0);
    }
}

template <class T>
int dc_blocker_cpu<T>::group_delay()
{
    if (d_long_form)
        return (2 * d_length - 2);
    else
        return d_length - 1;
}

template <class T>
work_return_code_t dc_blocker_cpu<T>::work(work_io& wio)
{

    auto in = wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;

    if (d_long_form) {
        T y1, y2, y3, y4, d;
        for (size_t i = 0; i < noutput_items; i++) {
            y1 = d_ma_0.filter(in[i]);
            y2 = d_ma_1.filter(y1);
            y3 = d_ma_2->filter(y2);
            y4 = d_ma_3->filter(y3);

            d_delay_line.push_back(d_ma_0.delayed_sig());
            d = d_delay_line[0];
            d_delay_line.pop_front();

            out[i] = d - y4;
        }
    }
    else {
        T y1, y2;
        for (size_t i = 0; i < noutput_items; i++) {
            y1 = d_ma_0.filter(in[i]);
            y2 = d_ma_1.filter(y1);
            out[i] = d_ma_0.delayed_sig() - y2;
        }
    }

    wio.outputs()[0].n_produced = noutput_items;
    return work_return_code_t::WORK_OK;
}

} /* namespace filter */
} /* namespace gr */
