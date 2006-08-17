/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_MESSAGE_H
#define INCLUDED_MB_MESSAGE_H

#include <mb_common.h>

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
mb_make_message(pmt_t signal,
		pmt_t data = PMT_NIL,
		pmt_t metadata = PMT_NIL,
		mb_pri_t priority = MB_PRI_DEFAULT);

class mb_message {
  pmt_t		d_signal;
  pmt_t		d_data;
  pmt_t		d_metadata;
  mb_pri_t	d_priority;
  // foo	d_rcvd_port_id;

  friend mb_message_sptr
  mb_make_message(pmt_t signal, pmt_t data, pmt_t metadata, mb_pri_t priority);

  // private constructor
  mb_message(pmt_t signal, pmt_t data, pmt_t metadata, mb_pri_t priority);

public:
  ~mb_message();

  pmt_t signal() const { return d_signal; }
  pmt_t data() const { return d_data; }
  pmt_t metadata() const { return d_metadata; }
  mb_pri_t priority() const { return d_priority; }
  // foo rcvd_port_id const { return d_rcvd_port_id; }
};

#endif /* INCLUDED_MB_MESSAGE_H */
