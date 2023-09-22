/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2008 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "realtime_impl.h"
#include <gnuradio/realtime.h>

namespace gr {

rt_status_t enable_realtime_scheduling()
{
    return gr::realtime::enable_realtime_scheduling();
}

} /* namespace gr */
