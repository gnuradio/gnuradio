/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

namespace gr {

enum class rt_status_t { OK = 0, NOT_IMPLEMENTED, NO_PRIVS, OTHER_ERROR };

enum class rt_sched_policy_t {
    RR = 0,   // round robin
    FIFO = 1, // first in first out
};

/*!
 * \brief If possible, enable high-priority "real time" scheduling.
 * \ingroup misc
 */
rt_status_t enable_realtime_scheduling();

} /* namespace gr */
