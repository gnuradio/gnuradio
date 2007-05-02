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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <mb_msg_queue.h>
#include <mb_message.h>


mb_msg_queue::mb_msg_queue()
  : d_not_empty(&d_mutex)
{
}

mb_msg_queue::~mb_msg_queue()
{
}

void
mb_msg_queue::insert(mb_message_sptr msg)
{
  omni_mutex_lock	l(d_mutex);
  
  mb_pri_t q = mb_pri_clamp(msg->priority());

  if (d_queue[q].empty_p()){
    d_queue[q].tail = d_queue[q].head = msg;
    msg->d_next.reset();	//msg->d_next = 0;
  }
  else {
    d_queue[q].tail->d_next = msg;
    d_queue[q].tail = msg;
    msg->d_next.reset();	// msg->d_next = 0;
  }

  // FIXME set bit in bitmap

  d_not_empty.signal();
}

/*
 * Delete highest pri message from the queue and return it.
 * Returns equivalent of zero pointer if queue is empty.
 *
 * Caller must be holding d_mutex
 */
mb_message_sptr
mb_msg_queue::get_highest_pri_msg_helper()
{
  // FIXME use bitmap and ffz to find best queue in O(1)

  for (mb_pri_t q = 0; q <= MB_PRI_WORST; q++){

    if (!d_queue[q].empty_p()){
      mb_message_sptr msg = d_queue[q].head;
      d_queue[q].head = msg->d_next;
      if (d_queue[q].head == 0){
	d_queue[q].tail.reset();	// d_queue[q].tail = 0;
	// FIXME clear bit in bitmap
      }

      msg->d_next.reset();		// msg->d_next = 0;
      return msg;
    }
  }

  return mb_message_sptr();		// eqv to a zero pointer
}


mb_message_sptr
mb_msg_queue::get_highest_pri_msg_nowait()
{
  omni_mutex_lock	l(d_mutex);

  return get_highest_pri_msg_helper();
}

mb_message_sptr
mb_msg_queue::get_highest_pri_msg()
{
  omni_mutex_lock l(d_mutex);

  while (1){
    mb_message_sptr msg = get_highest_pri_msg_helper();
    if (msg)			// Got one; return it
      return msg;

    d_not_empty.wait();		// Wait for something
  }
}

mb_message_sptr
mb_msg_queue::get_highest_pri_msg_timedwait(const mb_time &abs_time)
{
  unsigned long secs  = abs_time.d_secs;
  unsigned long nsecs = abs_time.d_nsecs;

  omni_mutex_lock l(d_mutex);

  while (1){
    mb_message_sptr msg = get_highest_pri_msg_helper();
    if (msg)			// Got one; return it
      return msg;

    if (!d_not_empty.timedwait(secs, nsecs))	// timed out
      return mb_message_sptr();			// eqv to zero pointer
  }
}
