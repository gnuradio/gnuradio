/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/kernel/api.h>
#include <deque>

namespace gr {
namespace kernel {
namespace filter {

template <class T>
class moving_averager
{
public:
    moving_averager(int D);

    T filter(T x);
    T delayed_sig() { return d_out; }

private:
    int d_length;
    T d_out, d_out_d1, d_out_d2;
    std::deque<T> d_delay_line;
};


} // namespace filter
} // namespace kernel
} /* namespace gr */
