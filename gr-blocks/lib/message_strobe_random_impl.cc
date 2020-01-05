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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "message_strobe_random_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

message_strobe_random::sptr
message_strobe_random::make(pmt::pmt_t msg,
                            message_strobe_random_distribution_t dist,
                            float mean_ms,
                            float std_ms)
{
    return gnuradio::get_initial_sptr(
        new message_strobe_random_impl(msg, dist, mean_ms, std_ms));
}


message_strobe_random_impl::message_strobe_random_impl(
    pmt::pmt_t msg,
    message_strobe_random_distribution_t dist,
    float mean_ms,
    float std_ms)
    : block("message_strobe_random",
            io_signature::make(0, 0, 0),
            io_signature::make(0, 0, 0)),
      d_rng(),
      d_mean_ms(mean_ms),
      d_std_ms(std_ms),
      d_dist(dist),
      pd(d_mean_ms),
      nd(d_mean_ms, d_std_ms),
      ud(d_mean_ms - d_std_ms, d_mean_ms + d_std_ms),
      d_finished(false),
      d_msg(msg),
      d_port(pmt::mp("strobe"))
{
    // set up ports
    message_port_register_out(d_port);
    d_thread = boost::shared_ptr<gr::thread::thread>(
        new gr::thread::thread(boost::bind(&message_strobe_random_impl::run, this)));

    message_port_register_in(pmt::mp("set_msg"));
    set_msg_handler(pmt::mp("set_msg"),
                    boost::bind(&message_strobe_random_impl::set_msg, this, _1));
}

void message_strobe_random_impl::set_mean(float mean_ms)
{
    d_mean_ms = mean_ms;
    pd = std::poisson_distribution<>(d_mean_ms);
    nd = std::normal_distribution<>(d_mean_ms, d_std_ms);
    ud = std::uniform_real_distribution<>(d_mean_ms - d_std_ms, d_mean_ms + d_std_ms);
}

float message_strobe_random_impl::mean() const { return d_mean_ms; }

void message_strobe_random_impl::set_std(float std_ms)
{
    d_std_ms = std_ms;
    nd = std::normal_distribution<>(d_mean_ms, d_std_ms);
    ud = std::uniform_real_distribution<>(d_mean_ms - d_std_ms, d_mean_ms + d_std_ms);
}

long message_strobe_random_impl::next_delay()
{
    switch (d_dist) {
    case STROBE_POISSON:
        // return d_variate_poisson->operator()();
        return static_cast<long>(pd(d_rng));
    case STROBE_GAUSSIAN:
        return static_cast<long>(nd(d_rng));
    case STROBE_UNIFORM:
        return static_cast<long>(ud(d_rng));
    default:
        throw std::runtime_error(
            "message_strobe_random_impl::d_distribution is very unhappy with you");
    }
}


message_strobe_random_impl::~message_strobe_random_impl()
{
    d_finished = true;
    d_thread->interrupt();
    d_thread->join();
}

void message_strobe_random_impl::run()
{
    while (!d_finished) {
        boost::this_thread::sleep(
            boost::posix_time::milliseconds(std::max(0L, next_delay())));
        if (d_finished) {
            return;
        }

        message_port_pub(d_port, d_msg);
    }
}

} /* namespace blocks */
} /* namespace gr */
