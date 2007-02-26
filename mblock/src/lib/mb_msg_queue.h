/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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
#ifndef INCLUDED_MB_MSG_QUEUE_H
#define INCLUDED_MB_MSG_QUEUE_H

#include <mb_common.h>
#include <omnithread.h>

/*!
 * \brief priority queue for mblock messages
 */
class mb_msg_queue : boost::noncopyable
{
  // When empty both head and tail are zero.
  struct subq {
    mb_message_sptr	head;
    mb_message_sptr	tail;

    bool empty_p() const { return head == 0; }
  };

  omni_mutex	d_mutex;

  // FIXME add bitmap to indicate which queues are non-empty.
  subq		d_queue[MB_NPRI];

public:
  mb_msg_queue();
  ~mb_msg_queue();

  //! Insert \p msg into priority queue.
  void insert(mb_message_sptr msg);

  /*
   * \brief Delete highest pri message from the queue and return it.
   * Returns equivalent of zero pointer if queue is empty.
   */
  mb_message_sptr get_highest_pri_msg();
};

#endif /* INCLUDED_MB_MSG_QUEUE_H */
