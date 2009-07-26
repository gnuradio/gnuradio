/* -*- c++ -*- */
/*
 * Copyright 2007,2009 Free Software Foundation, Inc.
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
#include <mb_timer_queue.h>

using namespace pmt;

static pmt_t
make_handle()
{
  static long counter = 0;
  pmt_t n = pmt_from_long(counter++);
  return pmt_list1(n);		// guaranteed to be a unique object
}

// one-shot constructor
mb_timeout::mb_timeout(const mb_time &abs_time,
		       pmt_t user_data, mb_msg_accepter_sptr accepter)
  : d_when(abs_time), d_is_periodic(false),
    d_user_data(user_data), d_handle(make_handle()), d_accepter(accepter)
{
}

// periodic constructor
mb_timeout::mb_timeout(const mb_time &first_abs_time, const mb_time &delta_time,
		       pmt_t user_data, mb_msg_accepter_sptr accepter)
  : d_when(first_abs_time), d_delta(delta_time), d_is_periodic(true),
    d_user_data(user_data), d_handle(make_handle()), d_accepter(accepter)
{
}

void
mb_timer_queue::cancel(pmt_t handle)
{
  container_type::iterator it;

  for (it = c.begin(); it != c.end();){
    if (pmt_equal((*it)->handle(), handle))
      it = c.erase(it);
    else
      ++it;
  }
  std::make_heap(c.begin(), c.end(), comp);
}
