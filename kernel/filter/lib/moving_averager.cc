/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/gr_complex.h>
#include <gnuradio/kernel/filter/moving_averager.h>

#include <cstdio>
#include <memory>

namespace gr {
namespace kernel {
namespace filter {


template <class T>
moving_averager<T>::moving_averager(int D)
    : d_length(D), d_out(0), d_out_d1(0), d_out_d2(0), d_delay_line(d_length - 1, 0)
{
}

template <class T>
T moving_averager<T>::filter(T x)
{
    d_out_d1 = d_out;
    d_delay_line.push_back(x);
    d_out = d_delay_line[0];
    d_delay_line.pop_front();

    T y = x - d_out_d1 + d_out_d2;
    d_out_d2 = y;

    return (y / (T)(d_length));
}


template class moving_averager<float>;
template class moving_averager<gr_complex>;

} // namespace filter
} // namespace kernel
} /* namespace gr */
