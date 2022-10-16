/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef PROBE_SIGNAL_IMPL_H
#define PROBE_SIGNAL_IMPL_H

#include <gnuradio/blocks/probe_signal.h>

namespace gr {
namespace blocks {

template <class T>
class probe_signal_impl : public probe_signal<T>
{
private:
    T d_level;
    mutable gr::thread::mutex d_mutex;

public:
    probe_signal_impl();
    ~probe_signal_impl() override;

    T level() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* PROBE_SIGNAL_IMPL_H */
