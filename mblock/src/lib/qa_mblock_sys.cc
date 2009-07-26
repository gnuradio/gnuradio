/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qa_mblock_sys.h>
#include <cppunit/TestAssert.h>
#include <mblock/mblock.h>
#include <mblock/runtime.h>
#include <mb_runtime_nop.h>		// QA only
#include <mblock/protocol_class.h>
#include <mblock/exception.h>
#include <mblock/msg_queue.h>
#include <mblock/message.h>
#include <mb_mblock_impl.h>
#include <mblock/msg_accepter.h>
#include <mblock/class_registry.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace pmt;

static pmt_t s_data    = pmt_intern("data");
static pmt_t s_status  = pmt_intern("status");
static pmt_t s_control = pmt_intern("control");
static pmt_t s_p0   = pmt_intern("p0");
static pmt_t s_p1   = pmt_intern("p1");
static pmt_t s_p2   = pmt_intern("p2");
static pmt_t s_p3   = pmt_intern("p3");
static pmt_t s_e1   = pmt_intern("e1");
static pmt_t s_r1   = pmt_intern("r1");

static void
define_protocol_classes()
{
  mb_make_protocol_class(s_data,		// name
			 pmt_list1(s_data),	// incoming
			 pmt_list1(s_data));	// outgoing
}


// ================================================================
//		          test_sys_1
// ================================================================

class sys_1 : public mb_mblock
{
  pmt_t		d_user_arg;
  mb_port_sptr	d_data;

public:
  sys_1(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~sys_1();
  void initial_transition();
};

sys_1::sys_1(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
    d_user_arg(user_arg)
{
  d_data = define_port("data", "data", true, mb_port::EXTERNAL);
}

sys_1::~sys_1(){}
  
void
sys_1::initial_transition()
{
  shutdown_all(d_user_arg);
}

REGISTER_MBLOCK_CLASS(sys_1);

void
qa_mblock_sys::test_sys_1()
{
  define_protocol_classes();

  pmt_t	result;
  pmt_t	n1 = pmt_from_long(1);
  pmt_t	n2 = pmt_from_long(2);

  mb_runtime_sptr rt1 = mb_make_runtime();

#if 0
  try {
    rt1->run("top-1", "sys_1", n1, &result);
  }
  catch (omni_thread_fatal e){
    std::cerr << "caught omni_thread_fatal: error = " << e.error
	      << ": " << strerror(e.error) << std::endl;
  }
  catch (omni_thread_invalid){
    std::cerr << "caught omni_thread_invalid\n";
  }
#else
    rt1->run("top-1", "sys_1", n1, &result);
#endif
  CPPUNIT_ASSERT(pmt_equal(n1, result));
  
  // Execute run a second time, with the same rt, to ensure sanity.
  rt1->run("top-2", "sys_1", n2, &result);
  CPPUNIT_ASSERT(pmt_equal(n2, result));
}

// ================================================================
//		          test_sys_2
// ================================================================

class squarer : public mb_mblock
{
  mb_port_sptr	d_data;

public:
  squarer(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);

  void handle_message(mb_message_sptr msg);
};

squarer::squarer(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_data = define_port("data", "data", true, mb_port::EXTERNAL);
}

void
squarer::handle_message(mb_message_sptr msg)
{
  if (!pmt_eq(msg->signal(), s_data))	// we only handle the "data" message
    return;

  // long x -> (long x . long (x * x))

  pmt_t x_pmt = msg->data();
  long x = pmt_to_long(x_pmt);
  d_data->send(s_data, pmt_cons(x_pmt, pmt_from_long(x * x)));
}

REGISTER_MBLOCK_CLASS(squarer);

// ----------------------------------------------------------------

class sys_2 : public mb_mblock
{
  mb_port_sptr	d_data;

public:
  sys_2(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  void initial_transition();
  void handle_message(mb_message_sptr msg);
};

sys_2::sys_2(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_data = define_port("data", "data", true, mb_port::INTERNAL);
  define_component("squarer", "squarer");
  connect("self", "data", "squarer", "data");
}

void
sys_2::initial_transition()
{
  // FIXME start timer to detect general failure

  d_data->send(s_data, pmt_from_long(0)); // send initial message
}

void
sys_2::handle_message(mb_message_sptr msg)
{
  if (!pmt_eq(msg->signal(), s_data))	// we only handle the "data" message
    return;

  // first check correctness of message

  long x = pmt_to_long(pmt_car(msg->data()));
  long y = pmt_to_long(pmt_cdr(msg->data()));

  // std::cout << msg->data() << std::endl;

  if (y != x * x){
    std::cerr << "sys_2::handle_message: Expected y == x * x.  Got y = "
	      << y << " for x = " << x << std::endl;

    shutdown_all(PMT_F);	// failed
  }

  if (x == 100)
    shutdown_all(PMT_T);			// done, OK
  else 
    d_data->send(s_data, pmt_from_long(x + 1));	// send next request
}

REGISTER_MBLOCK_CLASS(sys_2);

// ----------------------------------------------------------------

void
qa_mblock_sys::test_sys_2()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  // std::cerr << "qa_mblock_sys::test_sys_2 (enter)\n";
  
  rt->run("top-sys-2", "sys_2", PMT_F, &result);
  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

// ================================================================
//		          test_bitset_1
// ================================================================

void
qa_mblock_sys::test_bitset_1()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  long nmsgs =        1000;
  long batch_size =      8;
  
  pmt_t arg = pmt_list2(pmt_from_long(nmsgs),	// # of messages to send through pipe
			pmt_from_long(batch_size));

  rt->run("top", "qa_bitset_top", arg, &result);

  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

// ================================================================
//		          test_disconnect
// ================================================================

void
qa_mblock_sys::test_disconnect()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  long nmsgs =        10240;
  
  pmt_t arg = pmt_list1(pmt_from_long(nmsgs));	// # of messages to send through pipe


  rt->run("top", "qa_disconnect_top", arg, &result);

  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}
