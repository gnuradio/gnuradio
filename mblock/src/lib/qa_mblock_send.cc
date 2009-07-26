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

#include <qa_mblock_send.h>
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
  // Defined from client point-of-view.
  mb_make_protocol_class(pmt_intern("qa-send-cs"), 	// name
			 pmt_list1(s_status),		// incoming
			 pmt_list1(s_control));		// outgoing

}

mb_mblock_sptr
get_top(mb_runtime_sptr rts)
{
  return dynamic_cast<mb_runtime_nop *>(rts.get())->top();
}

// ================================================================
//		       test_simple_routing
// ================================================================

// sub-block for test_simple_routing

class sr1 : public mb_mblock
{
  mb_port_sptr	d_p1;
  mb_port_sptr	d_p2;
  mb_port_sptr	d_p3;

public:
  sr1(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~sr1();
  void initial_transition();
};

sr1::sr1(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_p1 = define_port("p1", "qa-send-cs", true, mb_port::EXTERNAL);
  d_p2 = define_port("p2", "qa-send-cs", true, mb_port::EXTERNAL);
  d_p3 = define_port("p3", "qa-send-cs", false, mb_port::EXTERNAL);
}

sr1::~sr1(){}
  
void
sr1::initial_transition()
{
  // std::cout << instance_name() << "[sr1]: initial_transition\n";

  // send two messages to each port
  pmt_t our_name = pmt_intern(instance_name());
  d_p1->send(s_status, pmt_list3(our_name, s_p1, pmt_from_long(0)));
  d_p1->send(s_status, pmt_list3(our_name, s_p1, pmt_from_long(1)));

  d_p2->send(s_status, pmt_list3(our_name, s_p2, pmt_from_long(0)));
  d_p2->send(s_status, pmt_list3(our_name, s_p2, pmt_from_long(1)));
}

REGISTER_MBLOCK_CLASS(sr1);

// ----------------------------------------------------------------

// top-level container block for test_simple_routing
class sr0 : public mb_mblock
{
  mb_port_sptr	d_p0;
  
public:
  sr0(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~sr0();
  void initial_transition();
};

sr0::sr0(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_p0 = define_port("p0", "qa-send-cs", false, mb_port::INTERNAL);

  define_component("mb1", "sr1");
  define_component("mb2", "sr1");

  connect("self", "p0", "mb1", "p1");
  connect("mb1", "p2", "mb2", "p3");
  connect("mb1", "p3", "mb2", "p2");
}

sr0::~sr0(){}

void
sr0::initial_transition()
{
  // std::cout << instance_name() << "[sr0]: initial_transition\n";

  // send two messages to p0
  pmt_t our_name = pmt_intern(instance_name());
  d_p0->send(s_control, pmt_list3(our_name, s_p0, pmt_from_long(0)));
  d_p0->send(s_control, pmt_list3(our_name, s_p0, pmt_from_long(1)));
}
  
REGISTER_MBLOCK_CLASS(sr0);

// ----------------------------------------------------------------

/*
 * This tests basic message routing using INTERNAL and EXTERNAL ports.
 * It does not rely on the guts of the runtime being complete,
 * which is good, because at the time this is being written, it isn't.
 */
void
qa_mblock_send::test_simple_routing()
{
  define_protocol_classes();

  mb_message_sptr msg;

  mb_runtime_sptr rt = mb_make_runtime_nop();
  rt->run("top", "sr0", PMT_F);

  mb_mblock_sptr mb0 = get_top(rt);
  
  // Reach into the guts and see if the messages ended up where they should have

  // mb0 should have received two messages sent from mb1 via its p1
  msg = mb0->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p0, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/mb1"), s_p1, pmt_from_long(0)),
			   msg->data()));

  msg = mb0->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p0, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/mb1"), s_p1, pmt_from_long(1)),
			   msg->data()));

  // mb1 should have received
  //   two messages from mb0 via its p0 and
  //   two messages from mb2 via its p3

  mb_mblock_sptr mb1 = mb0->impl()->component("mb1");

  msg = mb1->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p1, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top"), s_p0, pmt_from_long(0)),
			   msg->data()));

  msg = mb1->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p1, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top"), s_p0, pmt_from_long(1)),
			   msg->data()));

  msg = mb1->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p3, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/mb2"), s_p2, pmt_from_long(0)),
			   msg->data()));

  msg = mb1->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p3, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/mb2"), s_p2, pmt_from_long(1)),
			   msg->data()));


  // mb2 should have received
  //   two messages from mb2 via its p2

  mb_mblock_sptr mb2 = mb0->impl()->component("mb2");

  msg = mb2->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p3, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/mb1"), s_p2, pmt_from_long(0)),
			   msg->data()));

  msg = mb2->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p3, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/mb1"), s_p2, pmt_from_long(1)),
			   msg->data()));
}

// ================================================================
//		       test_relay_routing_1
// ================================================================

// internal block for test_relay_routing

class rr2 : public mb_mblock
{
  mb_port_sptr	d_p1;
  mb_port_sptr	d_p2;

public:
  rr2(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~rr2();
  void initial_transition();
};

rr2::rr2(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_p1 = define_port("p1", "qa-send-cs", true,  mb_port::EXTERNAL);
  d_p2 = define_port("p2", "qa-send-cs", false, mb_port::EXTERNAL);
}

rr2::~rr2(){}
  
void
rr2::initial_transition()
{
  // std::cout << instance_name() << "[rr2]: initial_transition\n";

  // send two messages via p1
  pmt_t our_name = pmt_intern(instance_name());
  d_p1->send(s_status, pmt_list3(our_name, s_p1, pmt_from_long(0)));
  d_p1->send(s_status, pmt_list3(our_name, s_p1, pmt_from_long(1)));
}

REGISTER_MBLOCK_CLASS(rr2);

// ----------------------------------------------------------------

// intermediate block for test_relay_routing

class rr1 : public mb_mblock
{
  mb_port_sptr	d_p1;
  mb_port_sptr	d_p2;

public:
  rr1(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~rr1();
};

rr1::rr1(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_p1 = define_port("p1", "qa-send-cs", true,  mb_port::RELAY);
  d_p2 = define_port("p2", "qa-send-cs", false, mb_port::RELAY);

  define_component("c0", "rr2");

  connect("self", "p1", "c0", "p1");
  connect("self", "p2", "c0", "p2");
}

rr1::~rr1(){}

REGISTER_MBLOCK_CLASS(rr1);

// ----------------------------------------------------------------

// top-level container for test_relay_routing

class rr0_a : public mb_mblock
{
public:
  rr0_a(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~rr0_a();
};

rr0_a::rr0_a(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  define_component("c0", "rr1");
  define_component("c1", "rr2");

  connect("c0", "p1", "c1", "p2");
  connect("c0", "p2", "c1", "p1");
}

rr0_a::~rr0_a(){}

REGISTER_MBLOCK_CLASS(rr0_a);

/*
 * This tests basic message routing using RELAY and EXTERNAL ports.
 * It does not rely on the guts of the runtime being complete,
 * which is good, because at the time this is being written, it isn't.
 */
void
qa_mblock_send::test_relay_routing_1()
{
  mb_message_sptr msg;

  mb_runtime_sptr rt = mb_make_runtime_nop();
  rt->run("top", "rr0_a", PMT_F);
  mb_mblock_sptr top = get_top(rt);

  // Reach into the guts and see if the messages ended up where they should have

  mb_mblock_sptr c0 = top->impl()->component("c0");
  mb_mblock_sptr c0c0 = c0->impl()->component("c0");

  mb_mblock_sptr c1 = top->impl()->component("c1");

  // c0c0 should have received
  //   two message from c1 via its p2

  msg = c0c0->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  //std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p2, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/c1"), s_p1, pmt_from_long(0)),
			   msg->data()));

  msg = c0c0->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  //std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p2, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/c1"), s_p1, pmt_from_long(1)),
			   msg->data()));

  // c1 should have received
  //   two message from c0c0 via its p2

  msg = c1->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  //std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p2, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/c0/c0"), s_p1, pmt_from_long(0)),
			   msg->data()));

  msg = c1->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  //std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p2, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/c0/c0"), s_p1, pmt_from_long(1)),
			   msg->data()));
}

// ================================================================
//		       test_relay_routing_2
// ================================================================

// top-level container for test_relay_routing_2

class rr0_b : public mb_mblock
{
public:
  rr0_b(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~rr0_b();
};

rr0_b::rr0_b(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  define_component("c0", "rr1");
  define_component("c1", "rr1");

  connect("c0", "p1", "c1", "p2");
  connect("c0", "p2", "c1", "p1");
}

rr0_b::~rr0_b(){}

REGISTER_MBLOCK_CLASS(rr0_b);

/*
 * This tests basic message routing using RELAY and EXTERNAL ports.
 * It does not rely on the guts of the runtime being complete,
 * which is good, because at the time this is being written, it isn't.
 */
void
qa_mblock_send::test_relay_routing_2()
{
  mb_message_sptr msg;

  mb_runtime_sptr rt = mb_make_runtime_nop();
  rt->run("top", "rr0_b", PMT_F);
  mb_mblock_sptr top = get_top(rt);

  // Reach into the guts and see if the messages ended up where they should have

  mb_mblock_sptr c0 = top->impl()->component("c0");
  mb_mblock_sptr c0c0 = c0->impl()->component("c0");

  mb_mblock_sptr c1 = top->impl()->component("c1");
  mb_mblock_sptr c1c0 = c1->impl()->component("c0");

  // c0c0 should have received
  //   two message from c1c0 via its p2

  msg = c0c0->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p2, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/c1/c0"), s_p1, pmt_from_long(0)),
			   msg->data()));

  msg = c0c0->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p2, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/c1/c0"), s_p1, pmt_from_long(1)),
			   msg->data()));

  // c1c0 should have received
  //   two message from c0c0 via its p2

  msg = c1c0->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p2, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/c0/c0"), s_p1, pmt_from_long(0)),
			   msg->data()));

  msg = c1c0->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(msg);
  // std::cerr << msg->data() << std::endl;
  CPPUNIT_ASSERT_EQUAL(s_p2, msg->port_id());
  CPPUNIT_ASSERT(pmt_equal(pmt_list3(pmt_intern("top/c0/c0"), s_p1, pmt_from_long(1)),
			   msg->data()));
}
