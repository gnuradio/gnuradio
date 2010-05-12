/* -*- c++ -*- */
/*
 * Copyright 2008,2010 Free Software Foundation, Inc.
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

#include "ring.h"

namespace usrp2 {

  ring::ring(unsigned int entries)
    : d_max(entries), d_read_ind(0), d_write_ind(0), d_ring(entries),
      d_mutex(), d_not_empty()
  {
    for (unsigned int i = 0; i < entries; i++) {
      d_ring[i].d_base = 0;
      d_ring[i].d_len = 0;
    }
  }

  void 
  ring::wait_for_not_empty() 
  { 
    gruel::scoped_lock l(d_mutex);
    while (empty()) 
      d_not_empty.wait(l);
  }

  bool
  ring::enqueue(void *p, size_t len)
  {
    gruel::scoped_lock l(d_mutex);
    if (full())
      return false;
      
    d_ring[d_write_ind].d_len = len;
    d_ring[d_write_ind].d_base = p;

    inc_write_ind();
    d_not_empty.notify_one();
    return true;
  }

  bool
  ring::dequeue(void **p, size_t *len)
  {
    gruel::scoped_lock l(d_mutex);
    if (empty())
      return false;
      
    *p   = d_ring[d_read_ind].d_base;
    *len = d_ring[d_read_ind].d_len;

    inc_read_ind();
    return true;
  }
  
} // namespace usrp2

