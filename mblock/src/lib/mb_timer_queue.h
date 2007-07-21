/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_MB_TIMER_QUEUE_H
#define INCLUDED_MB_TIMER_QUEUE_H

#include <mb_time.h>
#include <vector>
#include <queue>
#include <pmt.h>
#include <mb_msg_accepter.h>

class mb_timeout {
public:
  mb_time		d_when;		// absolute time to fire timeout
  mb_time		d_delta;	// if periodic, delta_t to next timeout
  bool			d_is_periodic;	// true iff this is a periodic timeout
  pmt_t			d_user_data;	// data from %timeout msg
  pmt_t			d_handle;	// handle for cancellation
  mb_msg_accepter_sptr	d_accepter;	// where to send the message

  // one-shot constructor
  mb_timeout(const mb_time &abs_time,
	     pmt_t user_data, mb_msg_accepter_sptr accepter);

  // periodic constructor
  mb_timeout(const mb_time &first_abs_time, const mb_time &delta_time,
	     pmt_t user_data, mb_msg_accepter_sptr accepter);

  pmt_t handle() const { return d_handle; }
};

typedef boost::shared_ptr<mb_timeout> mb_timeout_sptr;


//! Sort criterion for priority_queue
class timeout_later
{
public:
  bool operator() (const mb_timeout_sptr t1, const mb_timeout_sptr t2)
  {
    return t1->d_when > t2->d_when;
  }
};


class mb_timer_queue : public std::priority_queue<mb_timeout_sptr,
						  std::vector<mb_timeout_sptr>,
						  timeout_later>
{
public:
  void cancel(pmt_t handle);
};

#endif /* INCLUDED_MB_TIMER_QUEUE_H */
