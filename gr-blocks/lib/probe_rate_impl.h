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

#include <gnuradio/blocks/probe_rate.h>

namespace gr {
namespace blocks {

class probe_rate_impl : public probe_rate
{
private:
    double d_alpha, d_beta, d_avg;
    double d_min_update_time;
    boost::posix_time::ptime d_last_update;
    uint64_t d_lastthru;
    void setup_rpc();

    const pmt::pmt_t d_port;
    const pmt::pmt_t d_dict_avg, d_dict_now;

public:
    probe_rate_impl(size_t itemsize, double update_rate_ms, double alpha = 0.0001);
    ~probe_rate_impl();
    void set_alpha(double alpha);
    double rate();
    double timesincelast();
    bool start();
    bool stop();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

}; // end class

} /* namespace blocks */
} /* namespace gr */

#endif
