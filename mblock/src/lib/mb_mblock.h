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
#ifndef INCLUDED_MB_MBLOCK_H
#define INCLUDED_MB_MBLOCK_H

#include <mb_common.h>
#include <mb_message.h>
#include <mb_port.h>
#include <boost/enable_shared_from_this.hpp>


/*!
 * Abstract class implementing visitor pattern
 * \ingroup internal
 */
class mb_visitor
{
public:
  virtual ~mb_visitor();
  bool operator()(mb_mblock *mblock, const std::string &path) { return visit(mblock, path); }
  virtual bool visit(mb_mblock *mblock, const std::string &path) = 0;
};

// ----------------------------------------------------------------------

/*!
 * \brief Parent class for all message passing blocks
 *
 * Subclass this to define your mblocks.
 */
class mb_mblock : boost::noncopyable,
		  public boost::enable_shared_from_this<mb_mblock>
{
private:
  mb_mblock_impl_sptr	        d_impl;		// implementation details

  friend class mb_runtime;
  friend class mb_mblock_impl;

protected:
  /*!
   * \brief mblock constructor.
   *
   * Initializing all mblocks in the system is a 3 step procedure.
   *
   * The top level mblock's constructor is run.  That constructor (a)
   * registers all of its ports using define_port, (b) constructs and
   * registers any subcomponents it may have via the define_component
   * method, and then (c) issues connect calls to wire its
   * subcomponents together.
   */
  mb_mblock();

  /*!
   * \brief Called by the runtime system to execute the initial
   * transition of the finite state machine.
   *
   * Override this to initialize your finite state machine.
   */
  virtual void init_fsm();

  /*!
   * \brief Called by the runtime system when there's a message to handle.
   *
   * Override this to define your behavior.
   *
   * Do not issue any potentially blocking calls in this method.  This
   * includes things such reads or writes on sockets, pipes or slow
   * i/o devices.
   */
  virtual void handle_message(mb_message_sptr msg);

  /*!
   * \brief Define a port.
   *
   * EXTERNAL and RELAY ports are part of our peer interface.
   * INTERNAL ports are used to talk to sub-components.
   *
   * \param port_name    The name of the port (must be unique within this mblock).
   * \param protocol_class_name	The name of the protocol class associated with
   *				this port.  It must already be defined.
   * \param conjugated   Are the incoming and outgoing message sets swapped?
   * \param port_type    INTERNAL, EXTERNAL or RELAY.
   */
  mb_port_sptr
  define_port(const std::string &port_name,
	      const std::string &protocol_class_name,
	      bool conjugated,
	      mb_port::port_type_t port_type);

  /*!
   * \brief Define a subcomponent by name.
   *
   * Called within the constructor to tell the system the
   * names and identities of our sub-component mblocks.
   *
   * \param component_name  The name of the sub-component (must be unique with this mblock).
   * \param component       The sub-component instance.
   */
  void
  define_component(const std::string &component_name,
		   mb_mblock_sptr component);

  /*!
   * \brief connect endpoint_1 to endpoint_2
   *
   * \param comp_name1  component on one end of the connection
   * \param port_name1  the name of the port on comp1
   * \param comp_name2  component on the other end of the connection
   * \param port_name2  the name of the port on comp2
   *
   * An endpoint is specified by the component's local name (given as
   * component_name in the call to register_component) and the name of
   * the port on that component.
   *
   * To connect an internal or relay port, use "self" as the component name.
   */
  void
  connect(const std::string &comp_name1, const std::string &port_name1,
	  const std::string &comp_name2, const std::string &port_name2);

  /*!
   * \brief disconnect endpoint_1 from endpoint_2
   *
   * \param comp_name1  component on one end of the connection
   * \param port_name1  the name of the port on comp1
   * \param comp_name2  component on the other end of the connection
   * \param port_name2  the name of the port on comp2
   *
   * An endpoint is specified by the component's local name (given as
   * component_name in the call to register_component) and the name of
   * the port on that component.
   *
   * To disconnect an internal or relay port, use "self" as the component name.
   */
  void
  disconnect(const std::string &comp_name1, const std::string &port_name1,
	     const std::string &comp_name2, const std::string &port_name2);

  /*!
   * \brief disconnect all connections to specified component
   * \param component_name component to disconnect
   */
  void
  disconnect_component(const std::string component_name);

  /*!
   * \brief disconnect all connections to all components
   */
  void
  disconnect_all();

  /*!
   * \brief Return number of connections (QA mostly)
   */
  int
  nconnections() const;


public:
  virtual ~mb_mblock();

  /*!
   * \brief Perform a pre-order depth-first traversal of the hierarchy.
   *
   * The traversal stops and returns false if any call to visitor returns false.
   */
  bool
  walk_tree(mb_visitor *visitor, const std::string &path="");


  //! \implementation
  // internal use only
  mb_mblock_impl_sptr
  impl() const { return d_impl; }

};


#endif /* INCLUDED_MB_MBLOCK_H */
