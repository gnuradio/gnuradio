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

#include <qa_mblock_prims.h>
#include <cppunit/TestAssert.h>
#include <mblock/mblock.h>
#include <mblock/runtime.h>
#include <mblock/protocol_class.h>
#include <mblock/exception.h>
#include <mblock/msg_queue.h>
#include <mblock/message.h>
#include <mb_mblock_impl.h>
#include <mblock/msg_accepter.h>
#include <mblock/class_registry.h>
#include <stdio.h>

using namespace gruel;

static pmt_t s_cs = pmt_intern("cs");
static pmt_t s_debug = pmt_intern("debug");
static pmt_t s_in = pmt_intern("in");
static pmt_t s_out = pmt_intern("out");
  

// ================================================================

class dp_1 : public mb_mblock
{
public:
  dp_1(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~dp_1();
};

dp_1::dp_1(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
}

dp_1::~dp_1(){}

REGISTER_MBLOCK_CLASS(dp_1);

// ----------------------------------------------------------------

class dp_2 : public mb_mblock
{
public:
  dp_2(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~dp_2();
};

dp_2::dp_2(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  define_port("cs", "cs-protocol", false, mb_port::EXTERNAL);
}

dp_2::~dp_2(){}

REGISTER_MBLOCK_CLASS(dp_2);

// ----------------------------------------------------------------

class dp_3 : public mb_mblock
{
public:
  dp_3(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~dp_3();
};

dp_3::dp_3(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  define_port("cs", "cs-protocol", false, mb_port::EXTERNAL);
  define_port("cs", "cs-protocol", false, mb_port::EXTERNAL);	// duplicate def
}

dp_3::~dp_3(){}

REGISTER_MBLOCK_CLASS(dp_3);

// ----------------------------------------------------------------

void
qa_mblock_prims::test_define_ports()
{
  
  mb_runtime_sptr rts = mb_make_runtime();
  mb_runtime *rt = rts.get();
  
  // Should work
  mb_mblock_sptr  mb1 = mb_mblock_sptr(new dp_1(rt, "top", PMT_F));

  // raises runtime_error because of unknown protocol "cs-protocol"
  CPPUNIT_ASSERT_THROW(mb_mblock_sptr(new dp_2(rt, "top", PMT_F)),
		       std::runtime_error);

  // define the protocol class
  pmt_t pc = mb_make_protocol_class(pmt_intern("cs-protocol"),
				    pmt_list2(pmt_intern("start"),
					      pmt_intern("stop")),
				    PMT_NIL);

  // std::cout << "pc = " << pc << '\n';

  mb_mblock_sptr mb2 = mb_mblock_sptr(new dp_2(rt, "top", PMT_F));

  // raises pmt_exception because of duplicate port definition of "cs"
  CPPUNIT_ASSERT_THROW(mb_mblock_sptr(new dp_3(rt, "top", PMT_F)),
		       mbe_duplicate_port);
}

// ================================================================

class dc_0 : public mb_mblock
{
public:
  dc_0(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~dc_0();
};

dc_0::dc_0(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
}

dc_0::~dc_0() {}

REGISTER_MBLOCK_CLASS(dc_0);

// ----------------------------------------------------------------

class dc_ok : public mb_mblock
{
public:
  dc_ok(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~dc_ok();
};

dc_ok::dc_ok(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  define_component("c0", "dc_0");
  define_component("c1", "dc_0");
  define_component("c2", "dc_0");
}

dc_ok::~dc_ok(){}

REGISTER_MBLOCK_CLASS(dc_ok);

// ----------------------------------------------------------------

class dc_not_ok : public mb_mblock
{
public:
  dc_not_ok(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~dc_not_ok();
};

dc_not_ok::dc_not_ok(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  define_component("c0", "dc_0");
  define_component("c0", "dc_0");	// duplicate name
}

dc_not_ok::~dc_not_ok(){}

REGISTER_MBLOCK_CLASS(dc_not_ok);

// ----------------------------------------------------------------

void
qa_mblock_prims::test_define_components()
{
  mb_runtime_sptr rts = mb_make_runtime();
  mb_runtime *rt = rts.get();
  
  // Should work
  mb_mblock_sptr  mb1 = mb_mblock_sptr(new dc_ok(rt, "top", PMT_F));

  // raises pmt_exception because of duplicate component definition of "c0"
  CPPUNIT_ASSERT_THROW(mb_mblock_sptr(new dc_not_ok(rt, "top", PMT_F)),
		       mbe_duplicate_component);
}

// ================================================================

class tc_norm : public mb_mblock
{
public:
  tc_norm(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
    : mb_mblock(runtime, instance_name, user_arg)
  {
    define_port("data", "i/o", false, mb_port::EXTERNAL);
    define_port("norm", "i/o", false, mb_port::EXTERNAL);
    define_port("conj", "i/o", true,  mb_port::EXTERNAL);
    define_port("int",  "i/o", false, mb_port::INTERNAL);
  }

  ~tc_norm();
};

tc_norm::~tc_norm(){}

REGISTER_MBLOCK_CLASS(tc_norm);

////////////////////////////////////////////////////////////////

class tc_0 : public mb_mblock
{
public:
  tc_0(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
    : mb_mblock(runtime, instance_name, user_arg)
  {
    define_port("norm", "i/o", false, mb_port::EXTERNAL);
    define_port("conj", "i/o", true,  mb_port::EXTERNAL);
    define_port("int",  "i/o", false, mb_port::INTERNAL);

    define_component("c0", "tc_norm");
    define_component("c1", "tc_norm");
    define_component("c2", "tc_norm");
    define_component("c3", "tc_norm");
    define_component("c4", "tc_norm");
    define_component("c5", "tc_norm");

    // OK
    connect("c0", "norm", "c1", "conj");

    // No:  No such component name
    CPPUNIT_ASSERT_THROW(connect("foo", "data", "c1", "norm"), mbe_no_such_component);

    // No:  No such port name
    CPPUNIT_ASSERT_THROW(connect("c0", "data", "c1", "foo"), mbe_no_such_port);

    // No:  already connected
    CPPUNIT_ASSERT_THROW(connect("c0", "norm", "c2", "data"), mbe_already_connected);

    // No:  already connected
    CPPUNIT_ASSERT_THROW(connect("c2", "data", "c0", "norm"), mbe_already_connected);

    // No: incompatible ports
    CPPUNIT_ASSERT_THROW(connect("c1", "norm", "c2", "norm"), mbe_incompatible_ports);

    // OK
    connect("c1", "norm", "c2", "conj");

    // No: No such port name
    CPPUNIT_ASSERT_THROW(connect("c2", "norm", "self", "foo"), mbe_no_such_port);

    // No: can't connect to child's internal port
    CPPUNIT_ASSERT_THROW(connect("c0", "conj", "c2", "int"), mbe_no_such_port);

    // No: can't connect to our own external port
    CPPUNIT_ASSERT_THROW(connect("self", "norm", "c0", "conj"), mbe_invalid_port_type);

    // OK:  connecting to one of our internal ports
    connect("self", "int", "c3", "conj");

    // =====  Now test disconnecting some stuff =====

    // Confirm we're already connected
    CPPUNIT_ASSERT_THROW(connect("self", "int", "c3", "conj"), mbe_already_connected);

    int nc = nconnections();
    disconnect("self", "int", "c3", "conj");	// disconnect
    CPPUNIT_ASSERT_EQUAL(nc-1, nconnections());
    
    connect("self", "int", "c3", "conj");	// reconnect
    CPPUNIT_ASSERT_EQUAL(nc, nconnections());

    // confirm we're already connected
    CPPUNIT_ASSERT_THROW(connect("self", "int", "c3", "conj"), mbe_already_connected);


    connect("c0", "conj", "c5", "data");
    connect("c4", "norm", "c5", "conj");
    connect("c4", "conj", "c5", "norm");

    nc = nconnections();
    disconnect_component("c4");
    CPPUNIT_ASSERT_EQUAL(nc-2, nconnections());

    disconnect_component("c5");
    CPPUNIT_ASSERT_EQUAL(nc-3, nconnections());

    disconnect_all();
    CPPUNIT_ASSERT_EQUAL(0, nconnections());

  }

  ~tc_0();
};

tc_0::~tc_0(){}

REGISTER_MBLOCK_CLASS(tc_0);

////////////////////////////////////////////////////////////////

class tc_1 : public mb_mblock
{
public:
  tc_1(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
    : mb_mblock(runtime, instance_name, user_arg)
  {
    define_component("c0", "tc_norm");
    define_component("c1", "tc_norm");

    connect("c0", "norm", "c1", "conj");
  }

  ~tc_1();
};

tc_1::~tc_1(){}

REGISTER_MBLOCK_CLASS(tc_1);

////////////////////////////////////////////////////////////////

void
qa_mblock_prims::test_connect()
{
  // define the protocol class
  mb_make_protocol_class(pmt_intern("data"),			// name of class
			 pmt_list1(pmt_intern("data")),		// in
			 PMT_NIL);				// out

  mb_make_protocol_class(pmt_intern("i/o"),			// name of class
			 pmt_list1(pmt_intern("in")),		// in
			 pmt_list1(pmt_intern("out")));		// out

  mb_runtime_sptr rts = mb_make_runtime();
  mb_runtime *rt = rts.get();

  mb_mblock_sptr	mb0 = mb_mblock_sptr(new tc_0(rt, "top", PMT_F));
}

////////////////////////////////////////////////////////////////

void
qa_mblock_prims::test_msg_queue()
{
  mb_msg_queue	q;

  // check initial state
  CPPUNIT_ASSERT(q.get_highest_pri_msg_nowait() == 0);

  CPPUNIT_ASSERT(MB_NPRI >= 5);	// sanity check for this test

  // insert three messages at the same pri and ensure that they come out in order
  //                       signal       data          metadata     pri
  q.insert(mb_make_message(PMT_NIL, pmt_from_long(0), PMT_NIL, MB_PRI_BEST + 2));
  q.insert(mb_make_message(PMT_NIL, pmt_from_long(1), PMT_NIL, MB_PRI_BEST + 2));
  q.insert(mb_make_message(PMT_NIL, pmt_from_long(2), PMT_NIL, MB_PRI_BEST + 2));
  
  CPPUNIT_ASSERT_EQUAL(0L, pmt_to_long(q.get_highest_pri_msg_nowait()->data()));
  CPPUNIT_ASSERT_EQUAL(1L, pmt_to_long(q.get_highest_pri_msg_nowait()->data()));
  CPPUNIT_ASSERT_EQUAL(2L, pmt_to_long(q.get_highest_pri_msg_nowait()->data()));

  CPPUNIT_ASSERT(q.get_highest_pri_msg_nowait() == 0);


  // insert messages of different priorities in pseudo-random order
  //                       signal   data     metadata     pri
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 3));
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 2));
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 4));
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 0));
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 1));
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 3));
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 2));
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 4));
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 0));
  q.insert(mb_make_message(PMT_NIL, PMT_NIL, PMT_NIL, MB_PRI_BEST + 1));

  // confirm that they come out in order
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 0, q.get_highest_pri_msg_nowait()->priority());
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 0, q.get_highest_pri_msg_nowait()->priority());
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 1, q.get_highest_pri_msg_nowait()->priority());
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 1, q.get_highest_pri_msg_nowait()->priority());
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 2, q.get_highest_pri_msg_nowait()->priority());
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 2, q.get_highest_pri_msg_nowait()->priority());
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 3, q.get_highest_pri_msg_nowait()->priority());
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 3, q.get_highest_pri_msg_nowait()->priority());
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 4, q.get_highest_pri_msg_nowait()->priority());
  CPPUNIT_ASSERT_EQUAL(MB_PRI_BEST + 4, q.get_highest_pri_msg_nowait()->priority());
  
  // check final state
  CPPUNIT_ASSERT(q.get_highest_pri_msg_nowait() == 0);
}

////////////////////////////////////////////////////////////////

void
qa_mblock_prims::test_make_accepter()
{
  mb_runtime_sptr rts = mb_make_runtime();
  mb_runtime *rt = rts.get();

  // create a block
  mb_mblock_sptr mb = mb_mblock_sptr(new dp_2(rt, "top", PMT_F));

  // use "internal use only" method...
  mb_msg_accepter_sptr accepter = mb->impl()->make_accepter(pmt_intern("cs"));

  // Now push a few messages into it...
  //          signal       data          metadata     pri
  (*accepter)(PMT_NIL, pmt_from_long(0), PMT_NIL, MB_PRI_BEST + 2);
  (*accepter)(PMT_NIL, pmt_from_long(1), PMT_NIL, MB_PRI_BEST + 2);
  (*accepter)(PMT_NIL, pmt_from_long(2), PMT_NIL, MB_PRI_BEST + 2);

  // try to pull them out

  pmt_t cs = pmt_intern("cs");

  mb_message_sptr msg = mb->impl()->msgq().get_highest_pri_msg_nowait();
  CPPUNIT_ASSERT(pmt_eq(cs, msg->port_id()));	      // confirm that port_id is set
  CPPUNIT_ASSERT_EQUAL(0L, pmt_to_long(msg->data())); // and that data is correct

  CPPUNIT_ASSERT_EQUAL(1L, pmt_to_long(mb->impl()->msgq().get_highest_pri_msg_nowait()->data()));
  CPPUNIT_ASSERT_EQUAL(2L, pmt_to_long(mb->impl()->msgq().get_highest_pri_msg_nowait()->data()));
}
