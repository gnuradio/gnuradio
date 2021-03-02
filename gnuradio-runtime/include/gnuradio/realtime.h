/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_REALTIME_H
#define INCLUDED_GR_REALTIME_H

#include <gnuradio/api.h>

namespace gr {

typedef enum { RT_OK = 0, RT_NOT_IMPLEMENTED, RT_NO_PRIVS, RT_OTHER_ERROR } rt_status_t;

enum rt_sched_policy {
    RT_SCHED_RR = 0,   // round robin
    RT_SCHED_FIFO = 1, // first in first out
};

/*!
 * \brief If possible, enable high-priority "real time" scheduling.
 * \ingroup misc
 */
GR_RUNTIME_API rt_status_t enable_realtime_scheduling();

} /* namespace gr */

#endif /* INCLUDED_GR_REALTIME_H */
