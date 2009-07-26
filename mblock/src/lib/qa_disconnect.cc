/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
#include <mblock/mblock.h>
#include <mblock/protocol_class.h>
#include <mblock/message.h>
#include <mblock/class_registry.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <bitset>

using namespace pmt;

static pmt_t s_in = pmt_intern("in");
static pmt_t s_out = pmt_intern("out");
static pmt_t s_data = pmt_intern("data");
static pmt_t s_ack = pmt_intern("ack");
static pmt_t s_select_pipe = pmt_intern("select-pipe");
static pmt_t s_long0 = pmt_from_long(0);
static pmt_t s_sys_port = pmt_intern("%sys-port");
static pmt_t s_shutdown = pmt_intern("%shutdown");

class qa_disconnect_mux : public mb_mblock
{
  mb_port_sptr	d_in;
  mb_port_sptr	d_out;
  mb_port_sptr	d_cs;

public:
  qa_disconnect_mux(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  void initial_transition();
  void handle_message(mb_message_sptr msg);
};

qa_disconnect_mux::qa_disconnect_mux(mb_runtime *runtime,
				     const std::string &instance_name,
				     pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_in  = define_port("in", "qa-bitset", false, mb_port::RELAY);
  d_out = define_port("out", "qa-bitset", true, mb_port::RELAY);
  d_cs  = define_port("cs", "qa-disconnect-cs", true, mb_port::EXTERNAL);

  define_component("pipeline0", "qa_bitset8", pmt_from_long(0));
  define_component("pipeline1", "qa_bitset8", pmt_from_long(8));
}

void
qa_disconnect_mux::initial_transition(){}

void
qa_disconnect_mux::handle_message(mb_message_sptr msg)
{
  if (pmt_eq(msg->port_id(), d_cs->port_symbol())	// select-pipe on cs
      && pmt_eq(msg->signal(), s_select_pipe)){		

    long which_pipe = pmt_to_long(pmt_nth(0, msg->data()));

    disconnect_component("pipeline0");
    disconnect_component("pipeline1");

    switch(which_pipe){

    case 0:
      connect("self", "in",  "pipeline0", "in");
      connect("self", "out", "pipeline0", "out");
      break;

    case 1:
      connect("self", "in",  "pipeline1", "in");
      connect("self", "out", "pipeline1", "out");
      break;
    }

    d_cs->send(s_ack, msg->data());
    return;
  }
}

REGISTER_MBLOCK_CLASS(qa_disconnect_mux);

// ------------------------------------------------------------------------

class qa_disconnect_top : public mb_mblock
{
  enum state_t {
    UNINITIALIZED,
    WAIT_FOR_ACK,
    WAIT_FOR_DATA
  };

  state_t	d_state;
  int		d_msg_number;
  int		d_nmsgs_to_send;

  mb_port_sptr	d_in;
  mb_port_sptr	d_out;
  mb_port_sptr	d_cs;

  void check_pipe_send_next_msg();
  void send_next_msg();
  void select_pipe(int n);

  // alternate pipes every 128 messages
  static int  which_pipe(int msg_number) { return (msg_number >> 7) & 0x1; }
  bool time_to_switch() { return (d_msg_number & 0x7f) == 0; }
  
public:
  qa_disconnect_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  void initial_transition();
  void handle_message(mb_message_sptr msg);
};

qa_disconnect_top::qa_disconnect_top(mb_runtime *runtime,
				     const std::string &instance_name,
				     pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
    d_state(UNINITIALIZED), d_msg_number(0)
{
  d_nmsgs_to_send = pmt_to_long(pmt_nth(0, user_arg));

  d_in  = define_port("in", "qa-bitset", false, mb_port::INTERNAL);
  d_out = define_port("out", "qa-bitset", true, mb_port::INTERNAL);
  d_cs  = define_port("cs", "qa-disconnect-cs", false, mb_port::INTERNAL);

  define_component("mux", "qa_disconnect_mux", PMT_F);

  connect("self", "cs",  "mux", "cs");
  connect("self", "out", "mux", "in");
  connect("self", "in",  "mux", "out");
}

void
qa_disconnect_top::initial_transition()
{
  check_pipe_send_next_msg();
}

void
qa_disconnect_top::handle_message(mb_message_sptr msg)
{
  if (0)
    std::cerr << "qa_disconnect_top::handle_msg state = "
	      << d_state << "\n  msg = " << msg << std::endl;

  if (pmt_eq(msg->port_id(), d_cs->port_symbol())	// ack on cs
      && pmt_eq(msg->signal(), s_ack)
      && d_state == WAIT_FOR_ACK){

    send_next_msg();
    return;
  }

  if (pmt_eq(msg->port_id(), d_in->port_symbol())	// data on in
      && pmt_eq(msg->signal(), s_data)
      && d_state == WAIT_FOR_DATA){

    /*	
     * Confirm that msg passed through the pipe that we expect...
     */
    static const long expected_mask[2] = { 0x000000ff, 0x0000ff00 };

    long msg_number = pmt_to_long(pmt_car(msg->data()));
    long mask = pmt_to_long(pmt_cdr(msg->data()));

    if (mask != expected_mask[which_pipe(msg_number)]){
      fprintf(stderr, "\nqa_disconnect_top: wrong mask in msg_number = 0x%08lx\n",
	      msg_number);
      fprintf(stderr, "  expected = 0x%08lx, actual = 0x%08lx\n",
	      expected_mask[which_pipe(msg_number)], mask);
      shutdown_all(PMT_F);
      return;
    }

    if (msg_number == d_nmsgs_to_send - 1){	// we're done (and were successful)
      shutdown_all(PMT_T);
      return;
    }

    check_pipe_send_next_msg();
    return;
  }

  if (pmt_eq(msg->port_id(), s_sys_port)	// ignore %shutdown on %sys-port
      && pmt_eq(msg->signal(), s_shutdown))
    return;

  std::cerr << "qa_disconnect_top: unhandled msg: state = "
	    << d_state << "\n  msg = " << msg << std::endl;
}

void
qa_disconnect_top::select_pipe(int n)
{
  d_cs->send(s_select_pipe, pmt_list1(pmt_from_long(n)));
  d_state = WAIT_FOR_ACK;
}

void
qa_disconnect_top::send_next_msg()
{
  d_state = WAIT_FOR_DATA;
  if (d_msg_number == d_nmsgs_to_send)	// we've sent all we're supposed to
    return;

  d_out->send(s_data, pmt_cons(pmt_from_long(d_msg_number), s_long0));
  d_msg_number++;
}

void
qa_disconnect_top::check_pipe_send_next_msg()
{
  if (time_to_switch())
    select_pipe(which_pipe(d_msg_number));
  else
    send_next_msg();
}

REGISTER_MBLOCK_CLASS(qa_disconnect_top);
