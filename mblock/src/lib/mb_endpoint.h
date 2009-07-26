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

#ifndef INCLUDED_MB_ENDPOINT_H
#define INCLUDED_MB_ENDPOINT_H

#include <string>
#include <mblock/port.h>

/*!
 * \brief Endpoint specification for connection
 */
class mb_endpoint
{
  std::string	d_component_name;
  std::string	d_port_name;
  mb_port_sptr	d_port;			// the port object that this maps to

public:
  mb_endpoint(){}

  mb_endpoint(const std::string &component_name,
	      const std::string &port_name,
	      mb_port_sptr port)
    : d_component_name(component_name),
      d_port_name(port_name),
      d_port(port) {}

  const std::string &component_name() const { return d_component_name; }
  const std::string &port_name() const { return d_port_name; }
  mb_port_sptr port() const { return d_port; }

  //! Does this endpoint represent the inside of a relay port
  bool inside_of_relay_port_p() const;

  pmt::pmt_t	incoming_message_set() const;
  pmt::pmt_t	outgoing_message_set() const;
};

#endif /* INCLUDED_MB_ENDPOINT_H */
