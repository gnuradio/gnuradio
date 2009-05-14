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
#ifndef INCLUDED_MB_MBLOCK_H
#define INCLUDED_MB_MBLOCK_H

#include <mblock/common.h>
#include <mblock/message.h>
#include <mblock/port.h>
#include <mblock/time.h>


/*!
 * Abstract class implementing visitor pattern
 * \ingroup internal
 */
class mb_visitor
{
public:
  virtual ~mb_visitor();
  virtual bool operator()(mb_mblock *mblock) = 0;
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
  friend class mb_worker;

protected:
  /*!
   * \brief mblock constructor.
   *
   * Initializing all mblocks in the system is a 3 step procedure.
   *
   * The top level mblock's constructor is run.  That constructor 
   * (a) registers all of its ports using define_port, (b) registers any
   * subcomponents it may have via the define_component method, and
   * then (c) issues connect calls to wire its subcomponents together.
   *
   * \param runtime the runtime associated with this mblock
   * \param instance_name specify the name of this instance
   *        (for debugging, NUMA mapping, etc)
   * \param user_arg argument passed by user to constructor
   *        (ignored by the mb_mblock base class)
   */
  mb_mblock(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);

public:
  /*!
   * \brief Called by the runtime system to execute the initial
   * transition of the finite state machine.
   *
   * This method is called by the runtime after all blocks are
   * constructed and before the first message is delivered.  Override
   * this to initialize your finite state machine.
   */
  virtual void initial_transition();

protected:
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
   * \param class_name      The class of the instance that is to be created.
   * \param user_arg The argument to pass to the constructor of the component.
   */
  void
  define_component(const std::string &component_name,
		   const std::string &class_name,
		   pmt_t user_arg = PMT_NIL);

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
  disconnect_component(const std::string &component_name);

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

  //! Set the class name
  void set_class_name(const std::string &name);

  /*!
   * \brief Tell runtime that we are done.
   *
   * This method does not return.
   */
  void exit();

  /*!
   * \brief Ask runtime to execute the shutdown procedure for all blocks.
   * 
   * \param result sets value of \p result output argument of runtime->run(...)
   *
   * The runtime first sends a maximum priority %shutdown message to
   * all blocks.  All blocks should handle the %shutdown message,
   * perform whatever clean up is required, and call this->exit();
   *
   * After a period of time (~100ms), any blocks which haven't yet
   * called this->exit() are sent a maximum priority %halt message.
   * %halt is detected in main_loop, and this->exit() is called.
   *
   * After an additional period of time (~100ms), any blocks which
   * still haven't yet called this->exit() are sent a SIG<FOO> (TBD)
   * signal, which will blow them out of any blocking system calls and
   * raise an mbe_terminate exception.  The default top-level
   * runtime-provided exception handler will call this->exit() to
   * finish the process.
   *
   * runtime->run(...) returns when all blocks have called exit.
   */
  void shutdown_all(pmt_t result);

  /*!
   * \brief main event dispatching loop
   *
   * Although it is possible to override this, the default implementation
   * should work for virtually all cases.
   */
  virtual void main_loop();
  
public:
  virtual ~mb_mblock();

  //! Return instance name of this block
  std::string instance_name() const;

  //! Return the class name of this block
  std::string class_name() const;

  //! Set the instance name of this block.
  void set_instance_name(const std::string &name);
  
  //! Return the parent of this mblock, or 0 if we're the top-level block.
  mb_mblock *parent() const;

  /*!
   * \brief Schedule a "one shot" timeout.
   *
   * \param abs_time the absolute time at which the timeout should fire
   * \param user_data the data passed in the %timeout message.
   *
   * When the timeout fires, a message will be sent to the mblock.
   *
   * The message will have port_id = %sys-port, signal = %timeout,
   * data = user_data, metadata = the handle returned from
   * schedule_one_shot_timeout, pri = MB_PRI_BEST.
   *
   * \returns a handle that can be used in cancel_timeout, and is passed
   * as the metadata field of the generated %timeout message.
   *
   * To cancel a pending timeout, call cancel_timeout.
   */
  pmt_t
  schedule_one_shot_timeout(const mb_time &abs_time, pmt_t user_data);

  /*!
   * \brief Schedule a periodic timeout.
   *
   * \param first_abs_time The absolute time at which the first timeout should fire.
   * \param delta_time The relative delay between the first and successive timeouts.
   * \param user_data the data passed in the %timeout message.
   *
   * When the timeout fires, a message will be sent to the mblock, and a
   * new timeout will be scheduled for previous absolute time + delta_time.
   *
   * The message will have port_id = %sys-port, signal = %timeout,
   * data = user_data, metadata = the handle returned from
   * schedule_one_shot_timeout, pri = MB_PRI_BEST.
   *
   * \returns a handle that can be used in cancel_timeout, and is passed
   * as the metadata field of the generated %timeout message.
   *
   * To cancel a pending timeout, call cancel_timeout.
   */
  pmt_t
  schedule_periodic_timeout(const mb_time &first_abs_time,
			    const mb_time &delta_time,
			    pmt_t user_data);

  /*!
   * \brief Attempt to cancel a pending timeout.
   *
   * Note that this only stops a future timeout from firing.  It is
   * possible that a timeout may have already fired and enqueued a
   * %timeout message, but that that message has not yet been seen by
   * handle_message.
   *
   * \param handle returned from schedule_one_shot_timeout or schedule_periodic_timeout.
   */
  void cancel_timeout(pmt_t handle);

  /*!
   * \brief Perform a pre-order depth-first traversal of the hierarchy.
   *
   * The traversal stops and returns false if any call to visitor returns false.
   */
  bool
  walk_tree(mb_visitor *visitor);


  //! \internal
  // internal use only
  mb_mblock_impl_sptr
  impl() const { return d_impl; }

};


#endif /* INCLUDED_MB_MBLOCK_H */
