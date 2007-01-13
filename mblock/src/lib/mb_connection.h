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

#ifndef INCLUDED_MB_CONNECTION_H
#define INCLUDED_MB_CONNECTION_H

#include <mb_endpoint.h>
#include <list>

/*!
 * \brief Representation of a connection
 */
struct mb_connection {
  mb_endpoint	d_ep[2];

  mb_connection(const mb_endpoint &ep0, const mb_endpoint &ep1){
    d_ep[0] = ep0;
    d_ep[1] = ep1;
  }
};

typedef std::list<mb_connection>::iterator mb_conn_iter;
typedef std::list<mb_connection>::const_iterator mb_conn_const_iter;

/*!
 * \brief data structure that keeps track of connections
 */
class mb_conn_table {
  std::list<mb_connection> d_connections;

public:
  bool
  lookup_conn_by_name(const std::string &component_name,
		      const std::string &port_name,
		      mb_conn_iter *it, int *which_ep);

  bool
  lookup_conn_by_port(mb_port_sptr port,
		       mb_conn_iter *it, int *which_ep);

  void
  create_conn(const mb_endpoint &ep0, const mb_endpoint &ep1);


  void
  disconnect(const std::string &comp_name1, const std::string &port_name1,
	     const std::string &comp_name2, const std::string &port_name2);

  void
  disconnect_component(const std::string component_name);

  void
  disconnect_all();

  int
  nconnections() const;

};

#endif /* INCLUDED_MB_CONNECTION_H */
