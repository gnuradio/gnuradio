/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
#include <qa_timeouts.h>
#include <cppunit/TestAssert.h>
#include <mb_mblock.h>
#include <mb_runtime.h>
#include <mb_protocol_class.h>
#include <mb_exception.h>
#include <mb_message.h>
#include <mb_msg_accepter.h>
#include <mb_class_registry.h>
#include <mb_timer_queue.h>
#include <stdio.h>
#include <string.h>
#include <iostream>


static pmt_t s_timeout = pmt_intern("%timeout");
static pmt_t s_done = pmt_intern("done");


// ------------------------------------------------------------------------
//    Exercise the priority queue used to implement timeouts.
// ------------------------------------------------------------------------
void
qa_timeouts::test_timer_queue()
{
  mb_timer_queue	tq;
  mb_msg_accepter_sptr	accepter;

  mb_timeout_sptr	t1000_000 =
    mb_timeout_sptr(new mb_timeout(mb_time(1000,0), PMT_F, accepter));

  mb_timeout_sptr	t2000_000 =
    mb_timeout_sptr(new mb_timeout(mb_time(2000,0), PMT_F, accepter));
								    
  mb_timeout_sptr	t3000_000 =
    mb_timeout_sptr(new mb_timeout(mb_time(3000,0), PMT_F, accepter));
								    
  mb_timeout_sptr	t3000_125 =
    mb_timeout_sptr(new mb_timeout(mb_time(3000,125), PMT_F, accepter));
								    
  mb_timeout_sptr	t3000_250 =
    mb_timeout_sptr(new mb_timeout(mb_time(3000,250), PMT_F, accepter));
								    
  mb_timeout_sptr	t4000_000 =
    mb_timeout_sptr(new mb_timeout(mb_time(4000,0), PMT_F, accepter));
								    
  // insert in pseudo-random order

  tq.push(t3000_125);
  tq.push(t1000_000);
  tq.push(t4000_000);
  tq.push(t3000_250);
  tq.push(t2000_000);
  tq.push(t3000_000);

  CPPUNIT_ASSERT_EQUAL(t1000_000, tq.top());
  tq.pop();
  
  CPPUNIT_ASSERT_EQUAL(t2000_000, tq.top());
  tq.pop();
  
  CPPUNIT_ASSERT_EQUAL(t3000_000, tq.top());
  tq.pop();
  
  CPPUNIT_ASSERT_EQUAL(t3000_125, tq.top());
  tq.pop();
  
  CPPUNIT_ASSERT_EQUAL(t3000_250, tq.top());
  tq.pop();
  
  CPPUNIT_ASSERT_EQUAL(t4000_000, tq.top());
  tq.pop();

  CPPUNIT_ASSERT(tq.empty());

  // insert in pseudo-random order

  tq.push(t3000_000);
  tq.push(t4000_000);
  tq.push(t3000_125);
  tq.push(t1000_000);
  tq.push(t2000_000);
  tq.push(t3000_250);

  tq.cancel(t1000_000->handle());

  CPPUNIT_ASSERT_EQUAL(t2000_000, tq.top());
  tq.pop();
  
  CPPUNIT_ASSERT_EQUAL(t3000_000, tq.top());
  tq.pop();
  
  tq.cancel(t3000_250->handle());

  CPPUNIT_ASSERT_EQUAL(t3000_125, tq.top());
  tq.pop();
  
  CPPUNIT_ASSERT_EQUAL(t4000_000, tq.top());
  tq.pop();
  
  CPPUNIT_ASSERT(tq.empty());
}

// ------------------------------------------------------------------------
//   Test one-shot timeouts
// ------------------------------------------------------------------------

// FWIW, on SuSE 10.1 for x86-64, clock_getres returns 0.004 seconds.

#define TIMING_MARGIN 0.010	// seconds

class qa_timeouts_1_top : public mb_mblock
{
  int		d_nleft;
  int		d_nerrors;
  mb_time	d_t0;
  
public:
  qa_timeouts_1_top(mb_runtime *runtime,
		    const std::string &instance_name, pmt_t user_arg);

  void initial_transition();
  void handle_message(mb_message_sptr msg);
};

qa_timeouts_1_top::qa_timeouts_1_top(mb_runtime *runtime,
				     const std::string &instance_name,
				     pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
    d_nleft(0), d_nerrors(0)
{
}

void
qa_timeouts_1_top::initial_transition()
{
  d_t0 = mb_time::time();	// now

  schedule_one_shot_timeout(d_t0 + 0.200, pmt_from_double(0.200));
  schedule_one_shot_timeout(d_t0 + 0.125, pmt_from_double(0.125));
  schedule_one_shot_timeout(d_t0 + 0.075, pmt_from_double(0.075));
  schedule_one_shot_timeout(d_t0 + 0.175, pmt_from_double(0.175));

  d_nleft = 4;
}

void
qa_timeouts_1_top::handle_message(mb_message_sptr msg)
{
  if (pmt_eq(msg->signal(), s_timeout)){
    mb_time t_now = mb_time::time();
    double expected_delta_t = pmt_to_double(msg->data());
    double actual_delta_t = (t_now - d_t0).double_time();
    double delta = expected_delta_t - actual_delta_t;

    if (fabs(delta) > TIMING_MARGIN){
      std::cerr << "qa_timeouts_1_top: expected_delta_t = " << expected_delta_t
		<< " actual_delta_t = " << actual_delta_t << std::endl;
      d_nerrors++;
    }

    if (--d_nleft <= 0)
      shutdown_all(d_nerrors == 0 ? PMT_T : PMT_F);
  }
}

REGISTER_MBLOCK_CLASS(qa_timeouts_1_top);

void
qa_timeouts::test_timeouts_1()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  rt->run("top", "qa_timeouts_1_top", PMT_F, &result);

  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

// ------------------------------------------------------------------------
//   Test periodic timeouts
// ------------------------------------------------------------------------

class qa_timeouts_2_top : public mb_mblock
{
  int		d_nhandled;
  int		d_nerrors;
  double	d_delta_t;
  mb_time	d_t0;
  
public:
  qa_timeouts_2_top(mb_runtime *runtime,
		    const std::string &instance_name, pmt_t user_arg);

  void initial_transition();
  void handle_message(mb_message_sptr msg);
};

qa_timeouts_2_top::qa_timeouts_2_top(mb_runtime *runtime,
				     const std::string &instance_name,
				     pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
    d_nhandled(0), d_nerrors(0), d_delta_t(0.075)
{
}

void
qa_timeouts_2_top::initial_transition()
{
  d_t0 = mb_time::time();	// now

  schedule_periodic_timeout(d_t0 + d_delta_t, mb_time(d_delta_t), PMT_T);
}

void
qa_timeouts_2_top::handle_message(mb_message_sptr msg)
{
  static const int NMSGS_TO_HANDLE = 5;

  if (pmt_eq(msg->signal(), s_timeout)
      && !pmt_eq(msg->data(), s_done)){

    mb_time t_now = mb_time::time();

    d_nhandled++;

    double expected_delta_t = d_delta_t * d_nhandled;
    double actual_delta_t = (t_now - d_t0).double_time();
    double delta = expected_delta_t - actual_delta_t;

    if (fabs(delta) > TIMING_MARGIN){
      std::cerr << "qa_timeouts_2_top: expected_delta_t = " << expected_delta_t
		<< " actual_delta_t = " << actual_delta_t << std::endl;
      d_nerrors++;
    }

    if (d_nhandled == NMSGS_TO_HANDLE){
      cancel_timeout(msg->metadata());	// test cancel_timeout...
      schedule_one_shot_timeout(d_t0 + (d_delta_t * (d_nhandled + 2)), s_done);
    }
  }

  if (pmt_eq(msg->signal(), s_timeout)
      && pmt_eq(msg->data(), s_done)){
    if (d_nhandled != NMSGS_TO_HANDLE){
      std::cerr << "qa_timeouts_2_top: d_nhandled = " << d_nhandled
		<< " expected d_nhandled = " << NMSGS_TO_HANDLE
		<< " (cancel_timeout didn't work)\n";
      d_nerrors++;
    }
    shutdown_all(d_nerrors == 0 ? PMT_T : PMT_F);
  }
}

REGISTER_MBLOCK_CLASS(qa_timeouts_2_top);

void
qa_timeouts::test_timeouts_2()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  rt->run("top", "qa_timeouts_2_top", PMT_F, &result);

  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}
