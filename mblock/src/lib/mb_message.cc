/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2009 Free Software Foundation, Inc.
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
#include <mblock/message.h>
#include <stdio.h>
#include <gruel/pmt_pool.h>

static const int CACHE_LINE_SIZE = 64;	// good guess
static const int MAX_MESSAGES =  1024;	// KLUDGE max number of messages in sys
					//   0 -> no limit
#if MB_MESSAGE_LOCAL_ALLOCATOR

static pmt_pool 
global_msg_pool(sizeof(mb_message), CACHE_LINE_SIZE, 16*1024, MAX_MESSAGES);

void *
mb_message::operator new(size_t size)
{
  void *p = global_msg_pool.malloc();

  // fprintf(stderr, "mb_message::new p = %p\n", p);
  assert((reinterpret_cast<intptr_t>(p) & (CACHE_LINE_SIZE - 1)) == 0);
  return p;
}

void
mb_message::operator delete(void *p, size_t size)
{
  global_msg_pool.free(p);
}

#endif


mb_message_sptr
mb_make_message(gruel::pmt_t signal, gruel::pmt_t data, gruel::pmt_t metadata, mb_pri_t priority)
{
  return mb_message_sptr(new mb_message(signal, data, metadata, priority));
}

mb_message::mb_message(gruel::pmt_t signal, gruel::pmt_t data, gruel::pmt_t metadata, mb_pri_t priority)
  : d_signal(signal), d_data(data), d_metadata(metadata), d_priority(priority),
    d_port_id(gruel::PMT_NIL)
{
}

mb_message::~mb_message()
{
  // NOP
}

std::ostream& 
operator<<(std::ostream& os, const mb_message &msg)
{
  os << "<msg: signal=" << msg.signal()
     << " port_id=" << msg.port_id()
     << " data=" << msg.data()
     << " metadata=" << msg.metadata()
     << " pri=" << msg.priority()
     << ">";
  
  return os;
}
