/* -*- c++ -*- */
/*
 * Copyright 2005-2011,2013 Free Software Foundation, Inc.
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/streamops/throttle.h>
#include <atomic>
#include <chrono>

namespace gr {
namespace streamops {

class throttle_cpu : public throttle
{
public:
    throttle_cpu(block_args args);
    void set_sample_rate(double rate);

    bool start() override;
    work_return_code_t work(work_io&) override;

protected:
    double d_samps_per_sec;
    bool d_ignore_tags;

    std::chrono::time_point<std::chrono::steady_clock> d_start;
    uint64_t d_total_samples;
    double d_sample_rate;
    std::chrono::duration<double> d_sample_period;
};

} // namespace streamops
} // namespace gr