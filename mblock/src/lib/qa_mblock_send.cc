/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
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
#include <mb_mblock.h>
#include <mb_runtime.h>
#include <mb_runtime_nop.h>		// QA only
#include <mb_protocol_class.h>
#include <mb_exception.h>
#include <mb_msg_queue.h>
#include <mb_message.h>
#include <mb_mblock_impl.h>
#include <mb_msg_accepter.h>
#include <stdio.h>

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
  sr1();
  ~sr1();
  void init_fsm();
};

sr1::sr1()
{
  d_p1 = define_port("p1", "qa-send-cs", true, mb_port::EXTERNAL);
  d_p2 = define_port("p2", "qa-send-cs", true, mb_port::EXTERNAL);
  d_p3 = define_port("p3", "qa-send-cs", false, mb_port::EXTERNAL);
}

sr1::~sr1(){}
  
void
sr1::init_fsm()
{
  // std::cout << instance_name() << "[sr1]: init_fsm\n";

  // send two messages to each port
  pmt_t our_name = pmt_intern(instance_name());
  d_p1->send(s_status, pmt_list3(our_name, s_p1, pmt_from_long(0)));
  d_p1->send(s_status, pmt_list3(our_name, s_p1, pmt_from_long(1)));

  d_p2->send(s_status, pmt_list3(our_name, s_p2, pmt_from_long(0)));
  d_p2->send(s_status, pmt_list3(our_name, s_p2, pmt_from_long(1)));
}

// ----------------------------------------------------------------

// top-level container block for test_simple_routing
class sr0 : public mb_mblock
{
  mb_port_sptr	d_p0;
  
public:
  sr0();
  ~sr0();
  void init_fsm();
};

sr0::sr0()
{
  d_p0 = define_port("p0", "qa-send-cs", false, mb_port::INTERNAL);

  define_component("mb1", mb_mblock_sptr(new sr1()));
  define_component("mb2", mb_mblock_sptr(new sr1()));

  connect("self", "p0", "mb1", "p1");
  connect("mb1", "p2", "mb2", "p3");
  connect("mb1", "p3", "mb2", "p2");
}

sr0::~sr0(){}

void
sr0::init_fsm()
{
  // std::cout << instance_name() << "[sr0]: init_fsm\n";

  // send two messages to p0
  pmt_t our_name = pmt_intern(instance_name());
  d_p0->send(s_control, pmt_list3(our_name, s_p0, pmt_from_long(0)));
  d_p0->send(s_control, pmt_list3(our_name, s_p0, pmt_from_long(1)));
}
  
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
  mb_mblock_sptr mb0 = mb_mblock_sptr(new sr0());
  rt->run(mb0);

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
  rr2();
  ~rr2();
  void init_fsm();
};

rr2::rr2()
{
  d_p1 = define_port("p1", "qa-send-cs", true,  mb_port::EXTERNAL);
  d_p2 = define_port("p2", "qa-send-cs", false, mb_port::EXTERNAL);
}

rr2::~rr2(){}
  
void
rr2::init_fsm()
{
  // std::cout << instance_name() << "[rr2]: init_fsm\n";

  // send two messages via p1
  pmt_t our_name = pmt_intern(instance_name());
  d_p1->send(s_status, pmt_list3(our_name, s_p1, pmt_from_long(0)));
  d_p1->send(s_status, pmt_list3(our_name, s_p1, pmt_from_long(1)));
}

// ----------------------------------------------------------------

// intermediate block for test_relay_routing

class rr1 : public mb_mblock
{
  mb_port_sptr	d_p1;
  mb_port_sptr	d_p2;

public:
  rr1();
  ~rr1();
};

rr1::rr1()
{
  d_p1 = define_port("p1", "qa-send-cs", true,  mb_port::RELAY);
  d_p2 = define_port("p2", "qa-send-cs", false, mb_port::RELAY);

  define_component("c0", mb_mblock_sptr(new rr2()));

  connect("self", "p1", "c0", "p1");
  connect("self", "p2", "c0", "p2");
}

rr1::~rr1(){}

// ----------------------------------------------------------------

// top-level container for test_relay_routing

class rr0_a : public mb_mblock
{
public:
  rr0_a();
  ~rr0_a();
};

rr0_a::rr0_a()
{
  define_component("c0", mb_mblock_sptr(new rr1()));
  define_component("c1", mb_mblock_sptr(new rr2()));

  connect("c0", "p1", "c1", "p2");
  connect("c0", "p2", "c1", "p1");
}

rr0_a::~rr0_a(){}


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
  mb_mblock_sptr  top = mb_mblock_sptr(new rr0_a());
  rt->run(top);

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
  rr0_b();
  ~rr0_b();
};

rr0_b::rr0_b()
{
  define_component("c0", mb_mblock_sptr(new rr1()));
  define_component("c1", mb_mblock_sptr(new rr1()));

  connect("c0", "p1", "c1", "p2");
  connect("c0", "p2", "c1", "p1");
}

rr0_b::~rr0_b(){}


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
  mb_mblock_sptr  top = mb_mblock_sptr(new rr0_b());
  rt->run(top);

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
