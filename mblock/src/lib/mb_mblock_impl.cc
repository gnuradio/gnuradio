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
#include <mb_mblock_impl.h>
#include <mb_mblock.h>
#include <mb_protocol_class.h>
#include <mb_port.h>
#include <mb_exception.h>
#include <mb_util.h>


static pmt_t s_self = pmt_intern("self");

////////////////////////////////////////////////////////////////////////

bool 
mb_mblock_impl::port_is_defined(const std::string &name)
{
  return d_port_map.count(name) != 0;
}

bool
mb_mblock_impl::comp_is_defined(const std::string &name)
{
  return name == "self" || d_comp_map.count(name) != 0;
}

////////////////////////////////////////////////////////////////////////

mb_mblock_impl::mb_mblock_impl(mb_mblock *mb)
  : d_mb(mb)
{
}

mb_mblock_impl::~mb_mblock_impl()
{
  d_mb = 0;	// we don't own it
}


mb_port_sptr
mb_mblock_impl::define_port(const std::string &port_name,
			    const std::string &protocol_class_name,
			    bool conjugated,
			    mb_port::port_type_t port_type)
{
  if (port_type == mb_port::RELAY)
    throw mbe_base(d_mb, "mb_block_impl::define_port: RELAY ports are not implemented: " + port_name);
  
  if (port_is_defined(port_name))
    throw mbe_duplicate_port(d_mb, port_name);

  mb_port_sptr p = mb_port_sptr(new mb_port(port_name, protocol_class_name,
					    conjugated, port_type));
  d_port_map[port_name] = p;
  return p;
}

void
mb_mblock_impl::define_component(const std::string &name,
				 mb_mblock_sptr component)
{
  if (comp_is_defined(name))	// check for duplicate name
    throw mbe_duplicate_component(d_mb, name);

  d_comp_map[name] = component;
}

void
mb_mblock_impl::connect(const std::string &comp_name1, const std::string &port_name1,
			const std::string &comp_name2, const std::string &port_name2)
{
  mb_endpoint	ep0 = check_and_resolve_endpoint(comp_name1, port_name1);
  mb_endpoint	ep1 = check_and_resolve_endpoint(comp_name2, port_name2);

  if (!ports_are_compatible(ep0.port(), ep1.port()))
    throw mbe_incompatible_ports(d_mb,
				 comp_name1, port_name1,
				 comp_name2, port_name2);
  // FIXME more checks?

  d_conn_table.create_conn(ep0, ep1);
}

void
mb_mblock_impl::disconnect(const std::string &comp_name1, const std::string &port_name1,
			   const std::string &comp_name2, const std::string &port_name2)
{
  d_conn_table.disconnect(comp_name1, port_name1, comp_name2, port_name2);
}

void
mb_mblock_impl::disconnect_component(const std::string component_name)
{
  d_conn_table.disconnect_component(component_name);
}

void
mb_mblock_impl::disconnect_all()
{
  d_conn_table.disconnect_all();
}

int
mb_mblock_impl::nconnections() const
{
  return d_conn_table.nconnections();
}

////////////////////////////////////////////////////////////////////////

mb_endpoint
mb_mblock_impl::check_and_resolve_endpoint(const std::string &comp_name,
					   const std::string &port_name)
{
  mb_conn_iter	it;
  int		which_ep;
  mb_port_sptr	port = resolve_port(comp_name, port_name);

  // Confirm that we're not trying to connect to the inside of one of
  // our EXTERNAL ports.  Connections that include "self" as the
  // component name must be either INTERNAL or RELAY.

  if (comp_name == "self" && port->port_type() == mb_port::EXTERNAL)
    throw mbe_invalid_port_type(d_mb, comp_name, port_name);

  // Is this endpoint already connected?
  if (d_conn_table.lookup_conn_by_name(comp_name, port_name, &it, &which_ep))
    throw mbe_already_connected(d_mb, comp_name, port_name);

  return mb_endpoint(comp_name, port_name, port);
}

mb_port_sptr
mb_mblock_impl::resolve_port(const std::string &comp_name,
			     const std::string &port_name)
{
  if (comp_name == "self"){
    // Look through our ports.
    if (!port_is_defined(port_name))
      throw mbe_no_such_port(d_mb, mb_util::join_names("self", port_name));
    return d_port_map[port_name];
  }
  else {
    // Look through the specified child's ports.
    if (!comp_is_defined(comp_name))
      throw mbe_no_such_component(d_mb, comp_name);
    
    mb_mblock_impl_sptr  c_impl = d_comp_map[comp_name]->d_impl;  // childs impl pointer
    if (!c_impl->port_is_defined(port_name))
      throw mbe_no_such_port(d_mb, mb_util::join_names(comp_name, port_name));

    mb_port_sptr c_port = c_impl->d_port_map[port_name];

    if (c_port->port_type() == mb_port::INTERNAL)	      // can't "see" a child's internal ports
      throw mbe_no_such_port(d_mb, mb_util::join_names(comp_name, port_name));

    return c_port;
  }
}



bool
mb_mblock_impl::ports_are_compatible(mb_port_sptr p0, mb_port_sptr p1)
{
  using std::cout;
  using std::endl;

  pmt_t p0_outgoing = p0->outgoing_message_set();
  pmt_t p0_incoming = p0->incoming_message_set();

  pmt_t p1_outgoing = p1->outgoing_message_set();
  pmt_t p1_incoming = p1->incoming_message_set();

  return (pmt_subsetp(p0_outgoing, p1_incoming)
	  && pmt_subsetp(p1_outgoing, p0_incoming));
}

bool
mb_mblock_impl::walk_tree(mb_visitor *visitor, const std::string &path)
{
  if (!(*visitor)(d_mb, path))
    return false;

  mb_comp_map_t::iterator it;
  for (it = d_comp_map.begin(); it != d_comp_map.end(); ++it)
    if (!(it->second->walk_tree(visitor, path + "/" + it->first)))
      return false;

  return true;
}

