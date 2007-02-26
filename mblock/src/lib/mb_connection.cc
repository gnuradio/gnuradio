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

#include <mb_connection.h>

bool
mb_conn_table::lookup_conn_by_name(const std::string &component_name,
				   const std::string &port_name,
				   mb_conn_iter *itp, int *which_ep)
{
  mb_conn_iter end = d_connections.end();
  for (mb_conn_iter it = d_connections.begin(); it != end; ++it){

    if (it->d_ep[0].component_name() == component_name
	&& it->d_ep[0].port_name() == port_name){
      *itp = it;
      *which_ep = 0;
      return true;
    }

    if (it->d_ep[1].component_name() == component_name
	&& it->d_ep[1].port_name() == port_name){
      *itp = it;
      *which_ep = 1;
      return true;
    }
  }

  return false;
}

bool
mb_conn_table::lookup_conn_by_port(const mb_port *port,
				   mb_conn_iter *itp, int *which_ep)
{
  mb_conn_iter end = d_connections.end();
  for (mb_conn_iter it = d_connections.begin(); it != end; ++it){
    if (it->d_ep[0].port().get() == port){
      *itp = it;
      *which_ep = 0;
      return true;
    }
    if (it->d_ep[1].port().get() == port){
      *itp = it;
      *which_ep = 1;
      return true;
    }
  }

  return false;
}

void
mb_conn_table::create_conn(const mb_endpoint &ep0, const mb_endpoint &ep1)
{
  d_connections.push_back(mb_connection(ep0, ep1));
}

void
mb_conn_table::disconnect(const std::string &comp_name1, const std::string &port_name1,
			  const std::string &comp_name2, const std::string &port_name2)
{
  mb_conn_iter it;
  int	       which_ep;

  // look for comp_name1/port_name1
  bool found = lookup_conn_by_name(comp_name1, port_name1, &it, &which_ep);

  if (!found)	// no error if not found
    return;

  // FIXME if/when we do replicated ports, we may have one-to-many,
  // or many-to-many bindings.  For now, be paranoid
  assert(it->d_ep[which_ep^1].component_name() == comp_name2);
  assert(it->d_ep[which_ep^1].port_name() == port_name2);

  d_connections.erase(it);		// Poof!
}

void
mb_conn_table::disconnect_component(const std::string component_name)
{
  mb_conn_iter next;
  mb_conn_iter end = d_connections.end();
  for (mb_conn_iter it = d_connections.begin(); it != end; it = next){
    if (it->d_ep[0].component_name() == component_name
	|| it->d_ep[1].component_name() == component_name)
      next = d_connections.erase(it);	// Poof!
    else
      next = ++it;
  }
}

void
mb_conn_table::disconnect_all()
{
  d_connections.clear();		// All gone!
}

int
mb_conn_table::nconnections() const
{
  return d_connections.size();
}
