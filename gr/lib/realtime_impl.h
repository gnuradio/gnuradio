/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once

#include <gnuradio/realtime.h>

#include <stdexcept>

/*!
 * \brief System independent way to ask for realtime scheduling
 */
namespace gr {

/*
 * Define the range for our virtual priorities (don't change
 * these)
 *
 * Processes (or threads) with numerically higher priority values
 * are scheduled before processes with numerically lower priority
 * values. Thus, the value returned by rt_priority_max() will be
 * greater than the value returned by rt_priority_min().
 */
static inline int rt_priority_min() { return 0; }
static inline int rt_priority_max() { return 15; }
static inline int rt_priority_default() { return 1; }

struct rt_sched_param {
    int priority;
    rt_sched_policy policy;

    rt_sched_param() : priority(rt_priority_default()), policy(RT_SCHED_RR) {}

    rt_sched_param(int priority_, rt_sched_policy policy_ = RT_SCHED_RR)
    {
        if (priority_ < rt_priority_min() || priority_ > rt_priority_max())
            throw std::invalid_argument("rt_sched_param: priority out of range");

        priority = priority_;
        policy = policy_;
    }
};

rt_status_t enable_realtime_scheduling(rt_sched_param);

} /* namespace gr */
