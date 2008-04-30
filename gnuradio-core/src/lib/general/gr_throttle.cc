/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_throttle.h>
#include <gr_io_signature.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#if !defined(HAVE_NANOSLEEP) && defined(HAVE_SSLEEP)
#include <windows.h>
#endif


#ifdef HAVE_NANOSLEEP
void
gr_nanosleep(struct timespec *ts)
{
  struct timespec	*req = ts;
  struct timespec	rem;
  int r = nanosleep(req, &rem);
  while (r < 0 && errno == EINTR){
    req = &rem;
    r = nanosleep(req, &rem);
  }
  if (r < 0)
    perror ("gr_nanosleep");
}
#endif

gr_throttle_sptr
gr_make_throttle(size_t itemsize, double samples_per_sec)
{
  return gr_throttle_sptr(new gr_throttle(itemsize, samples_per_sec));
}

gr_throttle::gr_throttle(size_t itemsize, double samples_per_sec)
  : gr_sync_block("throttle",
		  gr_make_io_signature(1, 1, itemsize),
		  gr_make_io_signature(1, 1, itemsize)),
    d_itemsize(itemsize), d_samples_per_sec(samples_per_sec),
    d_total_samples(0)
{
#ifdef HAVE_GETTIMEOFDAY
  gettimeofday(&d_start, 0);
#endif  
}

gr_throttle::~gr_throttle()
{
}

int
gr_throttle::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  const char *in = (const char *) input_items[0];
  char *out = (char *) output_items[0];

#if defined(HAVE_GETTIMEOFDAY)
  //
  // If our average sample rate exceeds our target sample rate,
  // delay long enough to reduce to our target rate.
  //
  struct timeval now;
  gettimeofday(&now, 0);
  long t_usec = now.tv_usec - d_start.tv_usec;
  long t_sec  = now.tv_sec - d_start.tv_sec;
  double t = (double)t_sec + (double)t_usec * 1e-6;
  if (t < 1e-6)		// avoid unlikely divide by zero
    t = 1e-6;

  double actual_samples_per_sec = d_total_samples / t;
  if (actual_samples_per_sec > d_samples_per_sec){	// need to delay
    double delay = d_total_samples / d_samples_per_sec - t;
#ifdef HAVE_NANOSLEEP
    struct timespec ts;
    ts.tv_sec = (time_t)floor(delay);
    ts.tv_nsec = (long)((delay - floor(delay)) * 1e9);
    gr_nanosleep(&ts);
#elif HAVE_SSLEEP
    Sleep( (DWORD)(delay*1000) );
#endif
  }
#endif  

  memcpy(out, in, noutput_items * d_itemsize);
  d_total_samples += noutput_items;
  return noutput_items;
}
