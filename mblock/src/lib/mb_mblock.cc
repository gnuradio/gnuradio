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

#include <mb_mblock.h>
#include <mb_mblock_impl.h>


mb_mblock::mb_mblock()
  : d_impl(mb_mblock_impl_sptr(new mb_mblock_impl(this)))
{
}

mb_mblock::~mb_mblock()
{
  disconnect_all();

  // FIXME more?
}


void
mb_mblock::init_fsm()
{
  // default implementation does nothing
}

void
mb_mblock::handle_message(mb_message_sptr msg)
{
  // default implementation does nothing
}

////////////////////////////////////////////////////////////////////////
//           Forward other methods to implementation class            //
////////////////////////////////////////////////////////////////////////

mb_port_sptr
mb_mblock::define_port(const std::string &port_name_string,
		       const std::string &protocol_class_name,
		       bool conjugated,
		       mb_port::port_type_t port_type)
{
  return d_impl->define_port(port_name_string, protocol_class_name,
			     conjugated, port_type);
}

void
mb_mblock::define_component(const std::string &component_name,
			    mb_mblock_sptr component)
{
  d_impl->define_component(component_name, component);
}

void
mb_mblock::connect(const std::string &comp_name1, const std::string &port_name1,
		   const std::string &comp_name2, const std::string &port_name2)
{
  d_impl->connect(comp_name1, port_name1,
		  comp_name2, port_name2);
}				


void
mb_mblock::disconnect(const std::string &comp_name1, const std::string &port_name1,
		      const std::string &comp_name2, const std::string &port_name2)
{
  d_impl->disconnect(comp_name1, port_name1,
		     comp_name2, port_name2);
}

void
mb_mblock::disconnect_component(const std::string component_name)
{
  d_impl->disconnect_component(component_name);
}

void
mb_mblock::disconnect_all()
{
  d_impl->disconnect_all();
}

int
mb_mblock::nconnections() const
{
  return d_impl->nconnections();
}

bool
mb_mblock::walk_tree(mb_visitor *visitor, const std::string &path)
{
  return d_impl->walk_tree(visitor, path);
}
