/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MESSAGE_STROBE_RANDOM_IMPL_H
#define INCLUDED_GR_MESSAGE_STROBE_RANDOM_IMPL_H

#include <gnuradio/blocks/message_strobe_random.h>

#include <atomic>
#include <random>

namespace gr {
namespace blocks {

class BLOCKS_API message_strobe_random_impl : public message_strobe_random
{
private:
    std::mt19937 d_rng;
    float d_mean_ms;
    float d_std_ms;
    message_strobe_random_distribution_t d_dist;
    std::poisson_distribution<> pd;      //(d_mean_ms);
    std::normal_distribution<> nd;       //(d_mean_ms, d_std_ms);
    std::uniform_real_distribution<> ud; //(d_mean_ms - d_std_ms, d_mean_ms + d_std_ms);
    std::exponential_distribution<> ed;  //(1.0f / d_mean_ms);
    gr::thread::thread d_thread;
    std::atomic<bool> d_finished;
    pmt::pmt_t d_msg;
    const pmt::pmt_t d_port;

    void run();
    long next_delay();

public:
    message_strobe_random_impl(pmt::pmt_t msg,
                               message_strobe_random_distribution_t dist,
                               float mean_ms,
                               float std_ms);
    ~message_strobe_random_impl() override;

    void set_msg(pmt::pmt_t msg) override { d_msg = msg; }
    pmt::pmt_t msg() const override { return d_msg; }
    void set_mean(float mean_ms) override;
    float mean() const override;
    void set_std(float std_ms) override;
    float std() const override { return d_std_ms; }
    void set_dist(message_strobe_random_distribution_t dist) override { d_dist = dist; }
    message_strobe_random_distribution_t dist() const override { return d_dist; }
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_STROBE_RANDOM_IMPL_H */
