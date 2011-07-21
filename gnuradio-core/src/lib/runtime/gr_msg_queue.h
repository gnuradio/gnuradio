/* -*- c++ -*- */
/*
 * Copyright 2005,2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_MSG_QUEUE_H
#define INCLUDED_GR_MSG_QUEUE_H

#include <gr_core_api.h>
#include <gr_msg_handler.h>
#include <gruel/thread.h>

class gr_msg_queue;
typedef boost::shared_ptr<gr_msg_queue> gr_msg_queue_sptr;

GR_CORE_API gr_msg_queue_sptr gr_make_msg_queue(unsigned int limit=0);

/*!
 * \brief thread-safe message queue
 * \ingroup misc
 */
class GR_CORE_API gr_msg_queue : public gr_msg_handler {

  gruel::mutex		    d_mutex;
  gruel::condition_variable d_not_empty;
  gruel::condition_variable d_not_full;
  gr_message_sptr	    d_head;
  gr_message_sptr	    d_tail;
  unsigned int		    d_count;    // # of messages in queue.
  unsigned int		    d_limit;    // max # of messages in queue.  0 -> unbounded

public:
  gr_msg_queue(unsigned int limit);
  ~gr_msg_queue();

  //! Generic msg_handler method: insert the message.
  void handle(gr_message_sptr msg) { insert_tail (msg); }

  /*!
   * \brief Insert message at tail of queue.
   * \param msg message
   *
   * Block if queue if full.
   */
  void insert_tail(gr_message_sptr msg);

  /*!
   * \brief Delete message from head of queue and return it.
   * Block if no message is available.
   */
  gr_message_sptr delete_head();

  /*!
   * \brief If there's a message in the q, delete it and return it.
   * If no message is available, return 0.
   */
  gr_message_sptr delete_head_nowait();
  
  //! Delete all messages from the queue
  void flush();

  //! is the queue empty?
  bool empty_p() const { return d_count == 0; }
  
  //! is the queue full?
  bool full_p() const { return d_limit != 0 && d_count >= d_limit; }
  
  //! return number of messages in queue
  unsigned int count() const { return d_count; }

  //! return limit on number of message in queue.  0 -> unbounded
  unsigned int limit() const { return d_limit; }

};

#endif /* INCLUDED_GR_MSG_QUEUE_H */
