/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
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

#include <gnuradio/messages/msg_queue.h>
#include <stdexcept>

namespace gr {
  namespace messages {

    msg_queue_sptr
    make_msg_queue(unsigned int limit)
    {
      return msg_queue_sptr(new msg_queue(limit));
    }

    msg_queue::msg_queue(unsigned int limit)
      : d_limit(limit)
    {
    }

    msg_queue::~msg_queue()
    {
      flush();
    }

    void
    msg_queue::insert_tail(pmt::pmt_t msg)
    {
      gr::thread::scoped_lock guard(d_mutex);

      while(full_p())
        d_not_full.wait(guard);

      d_msgs.push_back(msg);
      d_not_empty.notify_one();
    }

    pmt::pmt_t
    msg_queue::delete_head()
    {
      gr::thread::scoped_lock guard(d_mutex);

      while(empty_p())
        d_not_empty.wait(guard);

      pmt::pmt_t m(d_msgs.front());
      d_msgs.pop_front();

      if(d_limit > 0)		// Unlimited length queues never block on write
        d_not_full.notify_one();

      return m;
    }

    pmt::pmt_t
    msg_queue::delete_head_nowait()
    {
      gr::thread::scoped_lock guard(d_mutex);

      if(empty_p())
        return pmt::pmt_t();

      pmt::pmt_t m(d_msgs.front());
      d_msgs.pop_front();

      if(d_limit > 0)		// Unlimited length queues never block on write
        d_not_full.notify_one();

      return m;
    }

    void
    msg_queue::flush()
    {
      while(delete_head_nowait() != pmt::pmt_t())
        ;
    }

  } /* namespace messages */
} /* namespace gr */
