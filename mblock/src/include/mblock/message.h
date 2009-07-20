/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2008,2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_MESSAGE_H
#define INCLUDED_MB_MESSAGE_H

#include <mblock/common.h>
#include <iosfwd>

#define MB_MESSAGE_LOCAL_ALLOCATOR 0	// define to 0 or 1

class mb_message;
typedef boost::shared_ptr<mb_message> mb_message_sptr;

/*!
 * \brief construct a message and return boost::shared_ptr
 *
 * \param signal	identifier of the message
 * \param data		the data to be operated on
 * \param metadata	information about the data
 * \param priority	urgency
 */
mb_message_sptr
mb_make_message(gruel::pmt_t signal,
		gruel::pmt_t data = gruel::PMT_NIL,
		gruel::pmt_t metadata = gruel::PMT_NIL,
		mb_pri_t priority = MB_PRI_DEFAULT);

class mb_message {
  mb_message_sptr d_next;		// link field for msg queue
  gruel::pmt_t	  d_signal;
  gruel::pmt_t	  d_data;
  gruel::pmt_t	  d_metadata;
  mb_pri_t	  d_priority;
  gruel::pmt_t	  d_port_id;		// name of port msg was rcvd on (symbol)

  friend class mb_msg_queue;

  friend mb_message_sptr
  mb_make_message(gruel::pmt_t signal, gruel::pmt_t data, gruel::pmt_t metadata, mb_pri_t priority);

  // private constructor
  mb_message(gruel::pmt_t signal, gruel::pmt_t data, gruel::pmt_t metadata, mb_pri_t priority);

public:
  ~mb_message();

  gruel::pmt_t signal() const { return d_signal; }
  gruel::pmt_t data() const { return d_data; }
  gruel::pmt_t metadata() const { return d_metadata; }
  mb_pri_t priority() const { return d_priority; }
  gruel::pmt_t port_id() const { return d_port_id; }

  void set_port_id(gruel::pmt_t port_id){ d_port_id = port_id; }

#if (MB_MESSAGE_LOCAL_ALLOCATOR)
  void *operator new(size_t);
  void operator delete(void *, size_t);
#endif
};

std::ostream& operator<<(std::ostream& os, const mb_message &msg);

inline
std::ostream& operator<<(std::ostream& os, const mb_message_sptr msg)
{
  os << *(msg.get());
  return os;
}

#endif /* INCLUDED_MB_MESSAGE_H */
