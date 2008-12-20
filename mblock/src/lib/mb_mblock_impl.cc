/* -*- c++ -*- */
/*
 * Copyright 2006,2008 Free Software Foundation, Inc.
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
#include <mb_mblock_impl.h>
#include <mblock/mblock.h>
#include <mblock/protocol_class.h>
#include <mblock/port.h>
#include <mb_port_simple.h>
#include <mblock/exception.h>
#include <mb_util.h>
#include <mb_msg_accepter_smp.h>
#include <mbi_runtime_lock.h>
#include <iostream>


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

mb_mblock_impl::mb_mblock_impl(mb_runtime_base *runtime, mb_mblock *mb,
			       const std::string &instance_name)
  : d_runtime(runtime), d_mb(mb), d_mb_parent(0), 
    d_instance_name(instance_name), d_class_name("mblock")
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
  mbi_runtime_lock	l(this);

  if (port_is_defined(port_name))
    throw mbe_duplicate_port(d_mb, port_name);

  mb_port_sptr p =
    mb_port_sptr(new mb_port_simple(d_mb,
				    port_name, protocol_class_name,
				    conjugated, port_type));
  d_port_map[port_name] = p;
  return p;
}

void
mb_mblock_impl::define_component(const std::string &name,
				 const std::string &class_name,
				 pmt_t user_arg)
{
  {
    mbi_runtime_lock	l(this);

    if (comp_is_defined(name))	// check for duplicate name
      throw mbe_duplicate_component(d_mb, name);
  }

  // We ask the runtime to create the component so that it can worry about
  // mblock placement on a NUMA machine or on a distributed multicomputer

  mb_mblock_sptr component =
    d_runtime->create_component(instance_name() + "/" + name,
				class_name, user_arg);
  {
    mbi_runtime_lock	l(this);

    component->d_impl->d_mb_parent = d_mb;     // set component's parent link
    d_comp_map[name] = component;
  }
}

void
mb_mblock_impl::connect(const std::string &comp_name1,
			const std::string &port_name1,
			const std::string &comp_name2,
			const std::string &port_name2)
{
  mbi_runtime_lock	l(this);

  mb_endpoint	ep0 = check_and_resolve_endpoint(comp_name1, port_name1);
  mb_endpoint	ep1 = check_and_resolve_endpoint(comp_name2, port_name2);

  if (!endpoints_are_compatible(ep0, ep1))
    throw mbe_incompatible_ports(d_mb,
				 comp_name1, port_name1,
				 comp_name2, port_name2);
  // FIXME more checks?

  d_conn_table.create_conn(ep0, ep1);
}

void
mb_mblock_impl::disconnect(const std::string &comp_name1,
			   const std::string &port_name1,
			   const std::string &comp_name2,
			   const std::string &port_name2)
{
  mbi_runtime_lock	l(this);

  d_conn_table.disconnect(comp_name1, port_name1, comp_name2, port_name2);
  invalidate_all_port_caches();
}

void
mb_mblock_impl::disconnect_component(const std::string component_name)
{
  mbi_runtime_lock	l(this);

  d_conn_table.disconnect_component(component_name);
  invalidate_all_port_caches();
}

void
mb_mblock_impl::disconnect_all()
{
  mbi_runtime_lock	l(this);

  d_conn_table.disconnect_all();
  invalidate_all_port_caches();
}

int
mb_mblock_impl::nconnections()
{
  mbi_runtime_lock	l(this);

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

    if (c_port->port_type() == mb_port::INTERNAL) // can't "see" a child's internal ports
      throw mbe_no_such_port(d_mb, mb_util::join_names(comp_name, port_name));

    return c_port;
  }
}



bool
mb_mblock_impl::endpoints_are_compatible(const mb_endpoint &ep0,
					 const mb_endpoint &ep1)
{
  pmt_t p0_outgoing = ep0.outgoing_message_set();
  pmt_t p0_incoming = ep0.incoming_message_set();

  pmt_t p1_outgoing = ep1.outgoing_message_set();
  pmt_t p1_incoming = ep1.incoming_message_set();

  return (pmt_subsetp(p0_outgoing, p1_incoming)
	  && pmt_subsetp(p1_outgoing, p0_incoming));
}

bool
mb_mblock_impl::walk_tree(mb_visitor *visitor)
{
  if (!(*visitor)(d_mb))
    return false;

  mb_comp_map_t::iterator it;
  for (it = d_comp_map.begin(); it != d_comp_map.end(); ++it)
    if (!(it->second->walk_tree(visitor)))
      return false;

  return true;
}

mb_msg_accepter_sptr
mb_mblock_impl::make_accepter(pmt_t port_name)
{
  // FIXME this should probably use some kind of configurable factory
  mb_msg_accepter *ma =
    new mb_msg_accepter_smp(d_mb->shared_from_this(), port_name);

  return mb_msg_accepter_sptr(ma);
}

bool
mb_mblock_impl::lookup_other_endpoint(const mb_port *port, mb_endpoint *ep)
{
  mb_conn_iter	it;
  int		which_ep = 0;

  if (!d_conn_table.lookup_conn_by_port(port, &it, &which_ep))
    return false;
  
  *ep = it->d_ep[which_ep^1];
  return true;
}

mb_mblock_sptr
mb_mblock_impl::component(const std::string &comp_name)
{
  if (comp_name == "self")
    return d_mb->shared_from_this();

  if (d_comp_map.count(comp_name) == 0)
    return mb_mblock_sptr();	// null pointer

  return d_comp_map[comp_name];
}

void
mb_mblock_impl::set_instance_name(const std::string &name)
{
  d_instance_name = name;
}

void
mb_mblock_impl::set_class_name(const std::string &name)
{
  d_class_name = name;
}

/*
 * This is the "Big Hammer" port cache invalidator.
 * It invalidates _all_ of the port caches in the entire mblock tree.
 * It's overkill, but was simple to code.
 */
void
mb_mblock_impl::invalidate_all_port_caches()
{
  class invalidator : public mb_visitor
  {
  public:
    bool operator()(mb_mblock *mblock)
    {
      mb_mblock_impl_sptr impl = mblock->impl();
      mb_port_map_t::iterator it = impl->d_port_map.begin();
      mb_port_map_t::iterator end = impl->d_port_map.end();
      for (; it != end; ++it)
	it->second->invalidate_cache();
      return true;
    }
  };

  invalidator visitor;

  // Always true, except in early QA code
  if (runtime()->top())
    runtime()->top()->walk_tree(&visitor);
}
