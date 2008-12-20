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

#include <mblock/exception.h>
#include <mblock/mblock.h>
#include <mb_util.h>


mbe_base::mbe_base(mb_mblock *mb, const std::string &msg)
  : logic_error(msg)	// FIXME extract block class name and id and add to msg
{
}

mbe_not_implemented::mbe_not_implemented(mb_mblock *mb, const std::string &msg)
  : mbe_base(mb, "Not implemented: " + msg)
{
}

mbe_no_such_class::mbe_no_such_class(mb_mblock *mb, const std::string &class_name)
  : mbe_base(mb, "No such class: " + class_name)
{
}

mbe_no_such_component::mbe_no_such_component(mb_mblock *mb, const std::string &component_name)
  : mbe_base(mb, "No such component: " + component_name)
{
}


mbe_duplicate_component::mbe_duplicate_component(mb_mblock *mb, const std::string &component_name)
  : mbe_base(mb, "Duplicate component: " + component_name)
{
}

mbe_no_such_port::mbe_no_such_port(mb_mblock *mb, const std::string &port_name)
  : mbe_base(mb, "No such port: " + port_name)
{
}

mbe_duplicate_port::mbe_duplicate_port(mb_mblock *mb, const std::string &port_name)
  : mbe_base(mb, "Duplicate port: " + port_name)
{
}

mbe_already_connected::mbe_already_connected(mb_mblock *mb,
					     const std::string &comp_name,
					     const std::string &port_name)
  : mbe_base(mb, "Port already connected: " + mb_util::join_names(comp_name, port_name))
{
}



mbe_incompatible_ports::mbe_incompatible_ports(mb_mblock *mb,
					       const std::string &comp1_name,
					       const std::string &port1_name,
					       const std::string &comp2_name,
					       const std::string &port2_name)
  : mbe_base(mb, "Incompatible ports: "
	     + mb_util::join_names(comp1_name, port1_name) + " "
	     + mb_util::join_names(comp2_name, port2_name))
{
}

mbe_invalid_port_type::mbe_invalid_port_type(mb_mblock *mb,
					     const std::string &comp_name,
					     const std::string &port_name)
  : mbe_base(mb, "Invalid port type for connection: " + mb_util::join_names(comp_name, port_name))
{
}

mbe_mblock_failed::mbe_mblock_failed(mb_mblock *mb,
				     const std::string &msg)
  : mbe_base(mb, "Message block failed: " + msg)
{
}

mbe_terminate::mbe_terminate()
{
}

mbe_exit::mbe_exit()
{
}
