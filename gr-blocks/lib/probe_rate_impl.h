/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_PROBE_RATE_IMPL_H
#define INCLUDED_GR_PROBE_RATE_IMPL_H

#include "pmt/pmt.h"
#include <gnuradio/blocks/probe_rate.h>
#include <string_view>
#include <chrono>
#include <map>

namespace gr {
namespace blocks {

class probe_rate_impl : public probe_rate
{
private:
    double d_alpha, d_beta, d_avg;
    const double d_min_update_time;
    std::chrono::time_point<std::chrono::steady_clock> d_last_update;
    uint64_t d_lastthru;
    void setup_rpc() override;

    const pmt::pmt_t d_port;
    const pmt::pmt_t d_dict_avg, d_dict_now;
    std::map<pmt::pmt_t, pmt::pmt_t> d_data_dict;

public:
    probe_rate_impl(size_t itemsize,
                    double update_rate_ms,
                    double alpha = 0.0001,
                    std::string_view name = "");
    ~probe_rate_impl() override;
    void set_alpha(double alpha) override;
    void set_name(std::string_view name) override;
    double rate() override;
    double timesincelast();
    bool start() override;
    bool stop() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

}; // end class

} /* namespace blocks */
} /* namespace gr */

#endif
