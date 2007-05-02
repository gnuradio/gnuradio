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
#include <mb_runtime.h>
#include <mb_exception.h>
#include <iostream>


static pmt_t s_sys_port = pmt_intern("%sys-port");
static pmt_t s_halt = pmt_intern("%halt");

mb_visitor::~mb_visitor()
{
  // nop base case for virtual destructor.
}


mb_mblock::mb_mblock(mb_runtime *runtime,
		     const std::string &instance_name,
		     pmt_t user_arg)
  : d_impl(mb_mblock_impl_sptr(
	       new mb_mblock_impl(dynamic_cast<mb_runtime_base*>(runtime),
				  this, instance_name)))
{
}

mb_mblock::~mb_mblock()
{
}


void
mb_mblock::initial_transition()
{
  // default implementation does nothing
}

void
mb_mblock::handle_message(mb_message_sptr msg)
{
  // default implementation does nothing
}


void
mb_mblock::main_loop()
{
  while (1){
    mb_message_sptr msg;
    try {
      while (1){
	msg = impl()->msgq().get_highest_pri_msg();

	// check for %halt from %sys-port
	if (pmt_eq(msg->port_id(), s_sys_port) && pmt_eq(msg->signal(), s_halt))
	  exit();

	handle_message(msg);
      }
    }
    catch (pmt_exception e){
      std::cerr << "\nmb_mblock::main_loop: ignored pmt_exception: "
		<< e.what()
		<< "\nin mblock instance \"" << instance_name()
		<< "\" while handling message:"
		<< "\n    port_id = " << msg->port_id()
		<< "\n     signal = " << msg->signal()
		<< "\n       data = " << msg->data()
		<< "\n  metatdata = " << msg->metadata() << std::endl;
    }
  }
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
			    const std::string &class_name,
			    pmt_t user_arg)
		
{
  d_impl->define_component(component_name, class_name, user_arg);
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
mb_mblock::disconnect_component(const std::string &component_name)
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
mb_mblock::walk_tree(mb_visitor *visitor)
{
  return d_impl->walk_tree(visitor);
}

std::string
mb_mblock::instance_name() const
{
  return d_impl->instance_name();
}

void
mb_mblock::set_instance_name(const std::string &name)
{
  d_impl->set_instance_name(name);
}

std::string
mb_mblock::class_name() const
{
  return d_impl->class_name();
}

void
mb_mblock::set_class_name(const std::string &name)
{
  d_impl->set_class_name(name);
}

mb_mblock *
mb_mblock::parent() const
{
  return d_impl->mblock_parent();
}

void
mb_mblock::exit()
{
  throw mbe_exit();	// adios...
}

void
mb_mblock::shutdown_all(pmt_t result)
{
  d_impl->runtime()->request_shutdown(result);
}

pmt_t
mb_mblock::schedule_one_shot_timeout(const mb_time &abs_time, pmt_t user_data)
{
  mb_msg_accepter_sptr accepter = impl()->make_accepter(s_sys_port);
  return d_impl->runtime()->schedule_one_shot_timeout(abs_time, user_data,
						      accepter);
}

pmt_t
mb_mblock::schedule_periodic_timeout(const mb_time &first_abs_time,
				     const mb_time &delta_time,
				     pmt_t user_data)
{
  mb_msg_accepter_sptr accepter = impl()->make_accepter(s_sys_port);
  return d_impl->runtime()->schedule_periodic_timeout(first_abs_time,
						      delta_time,
						      user_data,
						      accepter);
}

void
mb_mblock::cancel_timeout(pmt_t handle)
{
  d_impl->runtime()->cancel_timeout(handle);
}

