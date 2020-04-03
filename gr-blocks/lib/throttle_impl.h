/* -*- c++ -*- */
/*
 * Copyright 2005-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_THROTTLE_IMPL_H
#define INCLUDED_GR_THROTTLE_IMPL_H

#include <gnuradio/blocks/throttle.h>
#include <chrono>

namespace gr {
namespace blocks {

class throttle_impl : public throttle
{
private:
    std::chrono::time_point<std::chrono::steady_clock> d_start;
    const size_t d_itemsize;
    uint64_t d_total_samples;
    double d_sample_rate;
    std::chrono::duration<double> d_sample_period;
    const bool d_ignore_tags;

public:
    throttle_impl(size_t itemsize, double samples_per_sec, bool ignore_tags = true);
    ~throttle_impl();

    // Overloading gr::block::start to reset timer
    bool start();

    void setup_rpc();

    void set_sample_rate(double rate);
    double sample_rate() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_THROTTLE_IMPL_H */
