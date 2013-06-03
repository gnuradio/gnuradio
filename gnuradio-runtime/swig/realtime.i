/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
