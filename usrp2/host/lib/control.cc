/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010 Free Software Foundation, Inc.
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

#include "control.h"
#include <iostream>
#include <gruel/thread.h>

namespace usrp2 {

  pending_reply::pending_reply(unsigned int rid, void *buffer, size_t len)
    : d_rid(rid), d_buffer(buffer), d_len(len), d_mutex(), d_cond(),
      d_complete(false)
  {
  }

  pending_reply::~pending_reply()
  {
    notify_completion(); // Needed?
  }

  int
  pending_reply::wait_for_completion(double secs)
  {
    gruel::scoped_lock l(d_mutex);
    boost::system_time to(gruel::get_new_timeout(secs));

    while (!d_complete) {
      if (!d_cond.timed_wait(l, to))
	return 0; // timed out
    }

    return 1;
  }

  void
  pending_reply::notify_completion()
  {
    gruel::scoped_lock l(d_mutex);
    d_complete = true;
    d_cond.notify_one();
  }
  
} // namespace usrp2

