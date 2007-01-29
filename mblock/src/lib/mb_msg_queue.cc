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

// FIXME turn this into a template so we can use it for the runq of mblocks too

mb_msg_queue::mb_msg_queue()
{
}

mb_msg_queue::~mb_msg_queue()
{
}

void
mb_msg_queue::insert(mb_message_sptr msg)
{
  // omni_mutex_lock	l(d_mutex);		FIXME
  
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
}

mb_message_sptr
mb_msg_queue::get_highest_pri_msg()
{
  // omni_mutex_lock	l(d_mutex);		FIXME

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

  return mb_message_sptr();	// equivalent of a zero pointer
}
