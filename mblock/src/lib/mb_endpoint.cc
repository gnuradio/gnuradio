/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
#include <mb_endpoint.h>

using namespace pmt;

bool
mb_endpoint::inside_of_relay_port_p() const
{
  return d_port->port_type() == mb_port::RELAY && d_component_name == "self";
}

pmt_t
mb_endpoint::incoming_message_set() const
{
  if (inside_of_relay_port_p())			// swap incoming and outgoing
    return port()->outgoing_message_set();
  else
    return port()->incoming_message_set();
}

pmt_t
mb_endpoint::outgoing_message_set() const
{
  if (inside_of_relay_port_p())			// swap incoming and outgoing
    return port()->incoming_message_set();
  else
    return port()->outgoing_message_set();
}
