/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/omni_time.h>
#include "control.h"
#include <iostream>

namespace usrp2 {

  pending_reply::pending_reply(unsigned int rid, void *buffer, size_t len)
    : d_rid(rid), d_mutex(), d_cond(&d_mutex), d_buffer(buffer), d_len(len)
  {
  }

  pending_reply::~pending_reply()
  {
    signal(); // Needed?
  }

  int
  pending_reply::wait(double secs)
  {
    omni_mutex_lock l(d_mutex);
    omni_time abs_timeout = omni_time::time(omni_time(secs));
    return d_cond.timedwait(abs_timeout.d_secs, abs_timeout.d_nsecs);
  }

  void
  pending_reply::signal()
  {
    d_cond.signal();
  }
  
} // namespace usrp2

