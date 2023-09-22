/* -*- c++ -*- */
/*
 * Copyright 2005-2011 Free Software Foundation, Inc.
 * Copyright 2022 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "gnuradio/thread/thread.h"
#include <chrono>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "throttle_impl.h"
#include <gnuradio/io_signature.h>
#include <algorithm>
#include <cstring>
#include <limits>
#include <thread>

pmt::pmt_t throttle_rx_rate_pmt(pmt::intern("rx_rate"));

namespace gr {
namespace blocks {

throttle::sptr throttle::make(size_t itemsize,
                              double samples_per_sec,
                              bool ignore_tags,
                              unsigned int maximum_items_per_chunk)
{
    return gnuradio::make_block_sptr<throttle_impl>(
        itemsize, samples_per_sec, ignore_tags, maximum_items_per_chunk);
}

throttle_impl::throttle_impl(size_t itemsize,
                             double samples_per_second,
                             bool ignore_tags,
                             unsigned int maximum_items_per_chunk)
    : sync_block("throttle",
                 io_signature::make(1, 1, itemsize),
                 io_signature::make(0, 1, itemsize)),
      d_itemsize(itemsize),
      d_ignore_tags(ignore_tags),
      d_maximum_items_per_chunk(maximum_items_per_chunk)
{
    set_sample_rate(samples_per_second);
}

throttle_impl::~throttle_impl() {}

bool throttle_impl::start()
{
    d_total_items = 0;
    d_start = std::chrono::steady_clock::now();
    return block::start();
}

void throttle_impl::set_sample_rate(double rate)
{
    gr::thread::scoped_lock lock(d_setlock);
    // changing the sample rate performs a reset of state parameters
    d_total_items = 0;
    d_start = std::chrono::steady_clock::now();

    d_sample_rate = rate;
    d_sample_period = std::chrono::duration<double>(1 / rate);
}

double throttle_impl::sample_rate() const { return d_sample_rate; }

void throttle_impl::set_maximum_items_per_chunk(unsigned int maximum_items_per_chunk)
{
    gr::thread::scoped_lock lock(d_setlock);
    d_maximum_items_per_chunk = maximum_items_per_chunk;
}
unsigned int throttle_impl::maximum_items_per_chunk() const
{
    return d_maximum_items_per_chunk;
}

int throttle_impl::work(int noutput_items,
                        gr_vector_const_void_star& input_items,
                        gr_vector_void_star& output_items)
{
    // We need this as unsigned everywhere, but GNU Radio 3 API gives it to us as signed
    unsigned int noutput = static_cast<unsigned int>(noutput_items);

    // check for updated rx_rate tag
    if (!d_ignore_tags) {
        uint64_t abs_N = nitems_read(0);
        std::vector<tag_t> all_tags;
        get_tags_in_range(all_tags, 0, abs_N, abs_N + noutput);
        for (const auto& tag : all_tags) {
            if (pmt::eq(tag.key, throttle_rx_rate_pmt)) {
                double new_rate = pmt::to_double(tag.value);
                set_sample_rate(new_rate);
            }
        }
    }


    // If d_maximum_items_per_chunk is set, we should be emitting things in
    // chunks of that. But we'll allow shorter emissions if noutput is lower, in
    // order to not block flowgraphs that might never produce enough items for a
    // single chunk.
    unsigned int items_per_chunk;
    if (d_maximum_items_per_chunk) {
        items_per_chunk = std::min(noutput, d_maximum_items_per_chunk);
    } else {
        items_per_chunk = noutput;
    }

    // iff we have a connected output, then copy
    if (!output_items.empty()) {
        // copy all samples output[i] <= input[i]
        std::memcpy(reinterpret_cast<char*>(output_items[0]),
                    reinterpret_cast<const char*>(input_items[0]),
                    items_per_chunk * d_itemsize);
    }

    const auto now = std::chrono::steady_clock::now();
    decltype(d_start + d_sample_period * 1U) expected_time;

    // calculate the point in time until which we should wait
    // protect against corruption of complex timepoint object when set_sample_rate is
    // called concurrently
    {
        gr::thread::scoped_lock lock(d_setlock);
        expected_time = d_start + d_sample_period * (d_total_items + items_per_chunk);
    }
    if (expected_time > now) {
        constexpr auto limit_duration =
            std::chrono::duration<double>(std::numeric_limits<long>::max());
        if (expected_time - now > limit_duration) {
            d_logger->error("WARNING: Throttle sleep time overflow! You are probably "
                            "using a very low sample rate.");
        }
        std::this_thread::sleep_until(expected_time);
    }

    // increase number of processes items
    // protect against concurrent writing in set_sample_rate
    {
        gr::thread::scoped_lock lock(d_setlock);
        d_total_items += items_per_chunk;
    }
    return items_per_chunk;
}

void throttle_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    d_rpc_vars.emplace_back(
        new rpcbasic_register_get<throttle, double>(alias(),
                                                    "sample_rate",
                                                    &throttle::sample_rate,
                                                    pmt::mp(0.0),
                                                    pmt::mp(100.0e6),
                                                    pmt::mp(0.0),
                                                    "Hz",
                                                    "Sample Rate",
                                                    RPC_PRIVLVL_MIN,
                                                    DISPTIME | DISPOPTSTRIP));

    d_rpc_vars.emplace_back(
        new rpcbasic_register_set<throttle, double>(alias(),
                                                    "sample_rate",
                                                    &throttle::set_sample_rate,
                                                    pmt::mp(0.0),
                                                    pmt::mp(100.0e6),
                                                    pmt::mp(0.0),
                                                    "Hz",
                                                    "Sample Rate",
                                                    RPC_PRIVLVL_MIN,
                                                    DISPTIME | DISPOPTSTRIP));
#endif /* GR_CTRLPORT */
}

} /* namespace blocks */
} /* namespace gr */
