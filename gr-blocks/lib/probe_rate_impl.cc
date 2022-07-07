/* -*- c++ -*- */
/*
 * Copyright 2005,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pmt/pmt.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "probe_rate_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

probe_rate::sptr probe_rate::make(size_t itemsize,
                                  double update_rate_ms,
                                  double alpha,
                                  std::string_view name)
{
    return gnuradio::make_block_sptr<probe_rate_impl>(
        itemsize, update_rate_ms, alpha, name);
}

probe_rate_impl::probe_rate_impl(size_t itemsize,
                                 double update_rate_ms,
                                 double alpha,
                                 std::string_view name)
    : sync_block("probe_rate",
                 io_signature::make(1, 1, itemsize),
                 io_signature::make(0, 0, itemsize)),
      d_alpha(alpha),
      d_beta(1.0 - alpha),
      d_avg(0),
      d_min_update_time(update_rate_ms),
      d_lastthru(0),
      d_port(pmt::mp("rate")),
      d_dict_avg(pmt::mp("rate_avg")),
      d_dict_now(pmt::mp("rate_now"))
{
    message_port_register_out(d_port);
    set_name(name);
}

void probe_rate_impl::set_name(std::string_view name)
{
    if (name.empty()) {
        d_data_dict.erase(pmt::mp("name"));
    } else {
        d_data_dict[pmt::mp("name")] = pmt::mp(name);
    }
}

probe_rate_impl::~probe_rate_impl() {}

int probe_rate_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    d_lastthru += noutput_items;
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> diff = now - d_last_update;
    double diff_ms = diff.count();
    if (diff_ms >= d_min_update_time) {
        double rate_this_update = d_lastthru * 1e3 / diff_ms;
        d_lastthru = 0;
        d_last_update = now;
        if (d_avg == 0) {
            d_avg = rate_this_update;
        } else {
            d_avg = rate_this_update * d_alpha + d_avg * d_beta;
        }
        d_data_dict[d_dict_avg] = pmt::mp(d_avg);
        d_data_dict[d_dict_now] = pmt::mp(rate_this_update);
        message_port_pub(d_port, pmt::dict_from_mapping(d_data_dict));
    }
    return noutput_items;
}

void probe_rate_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<probe_rate_impl, double>(alias(),
                                                           "rate_items",
                                                           &probe_rate_impl::rate,
                                                           pmt::mp(0),
                                                           pmt::mp(1e6),
                                                           pmt::mp(1),
                                                           "items/sec",
                                                           "Item rate",
                                                           RPC_PRIVLVL_MIN,
                                                           DISPTIME | DISPOPTSTRIP)));
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_get<probe_rate_impl, double>(
        alias(),
        "timesincelast",
        &probe_rate_impl::timesincelast,
        pmt::mp(0),
        pmt::mp(d_min_update_time * 2),
        pmt::mp(0),
        "ms",
        "Time since last update",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));
#endif
}

void probe_rate_impl::set_alpha(double alpha) { d_alpha = alpha; }

double probe_rate_impl::rate() { return d_avg; }

double probe_rate_impl::timesincelast()
{
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> diff = now - d_last_update;
    return diff.count();
}

bool probe_rate_impl::start()
{
    d_avg = 0;
    d_lastthru = 0;
    d_last_update = std::chrono::steady_clock::now();
    return true;
}

bool probe_rate_impl::stop() { return true; }


} /* namespace blocks */
} /* namespace gr */
