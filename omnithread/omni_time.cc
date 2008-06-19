/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
#include <omni_time.h>
#include <omnithread.h>
#include <math.h>
#include <assert.h>


omni_time::omni_time(double real_secs)
{
  double floor_secs = floor(real_secs);
  d_secs = (long) floor_secs;
  d_nsecs = (long) ((real_secs - floor_secs) * 1e9);	  // always positive
}

omni_time
omni_time::time(const omni_time &delta_t)
{
  unsigned long	abs_sec, abs_nsec;
  unsigned long rel_sec  = delta_t.d_secs;
  unsigned long rel_nsec = delta_t.d_nsecs;
  
  omni_thread::get_time(&abs_sec, &abs_nsec, rel_sec, rel_nsec);
  return omni_time(abs_sec, abs_nsec);
}


omni_time
operator+(const omni_time &x, const omni_time &y)
{
  omni_time r(x.d_secs + y.d_secs, x.d_nsecs + y.d_nsecs);
  while (r.d_nsecs >= 1000000000){
    r.d_nsecs -= 1000000000;
    r.d_secs++;
  }
  return r;
}

omni_time
operator-(const omni_time &x, const omni_time &y)
{
  // assert(!(x < y));

  omni_time r(x.d_secs - y.d_secs, x.d_nsecs - y.d_nsecs);
  while (r.d_nsecs < 0){
    r.d_nsecs += 1000000000;
    r.d_secs--;
  }
  return r;
}

omni_time
operator+(const omni_time &x, double y)
{
  return x + omni_time(y);
}

omni_time
operator-(const omni_time &x, double y)
{
  return x - omni_time(y);
}
