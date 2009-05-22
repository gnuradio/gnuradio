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
#ifndef INCLUDED_GR_THROTTLE_H
#define INCLUDED_GR_THROTTLE_H

#include <gr_sync_block.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

class gr_throttle;
typedef boost::shared_ptr<gr_throttle> gr_throttle_sptr;


gr_throttle_sptr gr_make_throttle(size_t itemsize, double samples_per_sec);

/*!
 * \brief throttle flow of samples such that the average rate does not exceed samples_per_sec.
 * \ingroup misc_blk
 *
 * input: one stream of itemsize; output: one stream of itemsize
 *
 * N.B. this should only be used in GUI apps where there is no other
 * rate limiting block.  It is not intended nor effective at precisely
 * controlling the rate of samples.  That should be controlled by a
 * source or sink tied to sample clock.  E.g., a USRP or audio card.
 */
class gr_throttle : public gr_sync_block
{
  friend gr_throttle_sptr gr_make_throttle(size_t itemsize, double samples_per_sec);
  size_t		d_itemsize;
  double		d_samples_per_sec;
  double		d_total_samples;
#ifdef HAVE_SYS_TIME_H
  struct timeval	d_start;
#endif

  gr_throttle(size_t itemsize, double samples_per_sec);

public:
  ~gr_throttle();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};
 

#endif /* INCLUDED_GR_THROTTLE_H */
