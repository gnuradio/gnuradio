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
    double d_sample_rate;
    std::chrono::duration<double> d_sample_period;
    const bool d_ignore_tags;
    unsigned int d_maximum_items_per_chunk;
    uint64_t d_total_items = 0;

public:
    throttle_impl(size_t itemsize,
                  double samples_per_sec,
                  bool ignore_tags = true,
                  unsigned int maximum_items_per_chunk = 0);
    ~throttle_impl() override;

    // Overloading gr::block::start to reset timer
    bool start() override;

    void setup_rpc() override;

    void set_sample_rate(double rate) override;
    double sample_rate() const override;

    void set_maximum_items_per_chunk(unsigned int maximum_items_per_chunk) override;
    unsigned int maximum_items_per_chunk() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_THROTTLE_IMPL_H */
