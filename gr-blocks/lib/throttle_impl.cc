/* -*- c++ -*- */
/*
 * Copyright 2005-2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "throttle_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>
#include <limits>
#include <thread>

pmt::pmt_t throttle_rx_rate_pmt(pmt::intern("rx_rate"));

namespace gr {
namespace blocks {

throttle::sptr throttle::make(size_t itemsize, double samples_per_sec, bool ignore_tags)
{
    return gnuradio::make_block_sptr<throttle_impl>(
        itemsize, samples_per_sec, ignore_tags);
}

throttle_impl::throttle_impl(size_t itemsize, double samples_per_second, bool ignore_tags)
    : sync_block("throttle",
                 io_signature::make(1, 1, itemsize),
                 io_signature::make(1, 1, itemsize)),
      d_itemsize(itemsize),
      d_ignore_tags(ignore_tags)
{
    set_sample_rate(samples_per_second);
}

throttle_impl::~throttle_impl() {}

bool throttle_impl::start()
{
    d_start = std::chrono::steady_clock::now();
    d_total_samples = 0;
    return block::start();
}

void throttle_impl::set_sample_rate(double rate)
{
    // changing the sample rate performs a reset of state params
    d_start = std::chrono::steady_clock::now();
    d_total_samples = 0;
    d_sample_rate = rate;
    d_sample_period = std::chrono::duration<double>(1 / rate);
}

double throttle_impl::sample_rate() const { return d_sample_rate; }

int throttle_impl::work(int noutput_items,
                        gr_vector_const_void_star& input_items,
                        gr_vector_void_star& output_items)
{
    // check for updated rx_rate tag
    if (!d_ignore_tags) {
        uint64_t abs_N = nitems_read(0);
        std::vector<tag_t> all_tags;
        get_tags_in_range(all_tags, 0, abs_N, abs_N + noutput_items);
        for (const auto& tag : all_tags) {
            if (pmt::eq(tag.key, throttle_rx_rate_pmt)) {
                double new_rate = pmt::to_double(tag.value);
                set_sample_rate(new_rate);
            }
        }
    }

    // copy all samples output[i] <= input[i]
    const char* in = (const char*)input_items[0];
    char* out = (char*)output_items[0];
    std::memcpy(out, in, noutput_items * d_itemsize);
    d_total_samples += noutput_items;

    auto now = std::chrono::steady_clock::now();
    auto expected_time = d_start + d_sample_period * d_total_samples;

    if (expected_time > now) {
        auto limit_duration =
            std::chrono::duration<double>(std::numeric_limits<long>::max());
        if (expected_time - now > limit_duration) {
            GR_LOG_ALERT(d_logger,
                         "WARNING: Throttle sleep time overflow! You "
                         "are probably using a very low sample rate.");
        }
        std::this_thread::sleep_until(expected_time);
    }

    return noutput_items;
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
