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

#ifndef INCLUDED_MSG_QUEUE_H
#define INCLUDED_MSG_QUEUE_H

#include <gnuradio/api.h>
#include <gnuradio/thread/thread.h>
#include <pmt/pmt.h>
#include <deque>

namespace gr {
  namespace messages {

    class msg_queue;
    typedef boost::shared_ptr<msg_queue> msg_queue_sptr;

    msg_queue_sptr make_msg_queue(unsigned int limit=0);

    /*!
     * \brief thread-safe message queue
     */
    class GR_RUNTIME_API msg_queue
    {
    private:
      gr::thread::mutex              d_mutex;
      gr::thread::condition_variable d_not_empty;
      gr::thread::condition_variable d_not_full;
      unsigned int  d_limit;  // max # of messages in queue.  0 -> unbounded

      std::deque<pmt::pmt_t> d_msgs;

    public:
      msg_queue(unsigned int limit);
      ~msg_queue();

      /*!
       * \brief Insert message at tail of queue.
       * \param msg message
       *
       * Block if queue if full.
       */
      void insert_tail(pmt::pmt_t msg);

      /*!
       * \brief Delete message from head of queue and return it.
       * Block if no message is available.
       */
      pmt::pmt_t delete_head();

      /*!
       * \brief If there's a message in the q, delete it and return it.
       * If no message is available, return pmt::pmt_t().
       */
      pmt::pmt_t delete_head_nowait();

      //! Delete all messages from the queue
      void flush();

      //! is the queue empty?
      bool empty_p() const { return d_msgs.empty(); }

      //! is the queue full?
      bool full_p() const { return d_limit != 0 && count() >= d_limit; }

      //! return number of messages in queue
      unsigned int count() const { return d_msgs.size(); }

      //! return limit on number of message in queue.  0 -> unbounded
      unsigned int limit() const { return d_limit; }
    };

  } /* namespace messages */
} /* namespace gr */

#endif /* INCLUDED_MSG_QUEUE_H */
