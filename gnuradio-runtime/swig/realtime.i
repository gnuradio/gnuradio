/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

%rename(enable_realtime_scheduling) gr::enable_realtime_scheduling;

// NOTE: This is duplicated from gnuradio-runtime/include/realtime.h,
//       and must be kept in sync with it.  This is the least evil workaround
//       for allowing 3rd party code builds to work when GNU Radio is
//       installed from binary packages into the standard system directories.
//       Otherwise, they can't find #include <realtime.h>, since
//       pkg-config strips -I/usr/include from the --cflags path.

namespace gr {

  typedef enum {
    RT_OK = 0,
    RT_NOT_IMPLEMENTED,
    RT_NO_PRIVS,
    RT_OTHER_ERROR
  } rt_status_t;

  gr::rt_status_t gr::enable_realtime_scheduling();
}
