/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_MESSAGE_STROBE_RANDOM_IMPL_H
#define INCLUDED_GR_MESSAGE_STROBE_RANDOM_IMPL_H

#include <gnuradio/blocks/message_strobe_random.h>

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
    boost::shared_ptr<gr::thread::thread> d_thread;
    bool d_finished;
    pmt::pmt_t d_msg;
    const pmt::pmt_t d_port;

    void run();
    long next_delay();

public:
    message_strobe_random_impl(pmt::pmt_t msg,
                               message_strobe_random_distribution_t dist,
                               float mean_ms,
                               float std_ms);
    ~message_strobe_random_impl();

    void set_msg(pmt::pmt_t msg) { d_msg = msg; }
    pmt::pmt_t msg() const { return d_msg; }
    void set_mean(float mean_ms);
    float mean() const;
    void set_std(float std_ms);
    float std() const { return d_std_ms; }
    void set_dist(message_strobe_random_distribution_t dist) { d_dist = dist; }
    message_strobe_random_distribution_t dist() const { return d_dist; }
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_STROBE_RANDOM_IMPL_H */
