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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <mb_port.h>
#include <mb_protocol_class.h>

mb_port::mb_port(mb_mblock *mblock,
		 const std::string &port_name,
		 const std::string &protocol_class_name,
		 bool conjugated,
		 mb_port::port_type_t port_type)
  : d_port_name(port_name), d_conjugated(conjugated), d_port_type(port_type),
    d_mblock(mblock)
{
  pmt_t pc = mb_protocol_class_lookup(pmt_intern(protocol_class_name));
  if (pmt_is_null(pc)){
    throw std::runtime_error("mb_port: unknown protocol class '"
			     + protocol_class_name + "'");
  }
  d_protocol_class = pc;
}

mb_port::~mb_port()
{
  // nop
}

pmt_t
mb_port::incoming_message_set() const
{
  if (!conjugated())
    return mb_protocol_class_incoming(protocol_class());
  else	// swap the sets
    return mb_protocol_class_outgoing(protocol_class());
}

pmt_t
mb_port::outgoing_message_set() const
{
  if (!conjugated())
    return mb_protocol_class_outgoing(protocol_class());
  else	// swap the sets
    return mb_protocol_class_incoming(protocol_class());
}
