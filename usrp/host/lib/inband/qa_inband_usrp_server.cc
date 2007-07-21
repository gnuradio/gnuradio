/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include <qa_inband_usrp_server.h>
#include <cppunit/TestAssert.h>
#include <stdio.h>
#include <usrp_server.h>
#include <mb_mblock.h>
#include <mb_runtime.h>
#include <mb_protocol_class.h>
#include <mb_class_registry.h>
#include <vector>
#include <iostream>

static pmt_t s_cmd_allocate_channel = pmt_intern("cmd-allocate-channel");
static pmt_t s_response_allocate_channel = pmt_intern("response-allocate-channel");
static pmt_t s_send_allocate_channel = pmt_intern("send-allocate-channel");
static pmt_t s_cmd_deallocate_channel = pmt_intern("cmd-deallocate-channel");
static pmt_t s_response_deallocate_channel = pmt_intern("response-deallocate-channel");
static pmt_t s_send_deallocate_channel = pmt_intern("send-deallocate-channel");
static pmt_t s_cmd_max_capacity = pmt_intern("cmd-max-capacity");
static pmt_t s_response_max_capacity = pmt_intern("response-max-capacity");
static pmt_t s_cmd_ntx_chan  = pmt_intern("cmd-ntx-chan");
static pmt_t s_cmd_nrx_chan  = pmt_intern("cmd-nrx-chan");
static pmt_t s_response_ntx_chan = pmt_intern("response-ntx-chan");
static pmt_t s_response_nrx_chan = pmt_intern("response-nrx-chan");
static pmt_t s_cmd_current_capacity_allocation  = pmt_intern("cmd-current-capacity-allocation");
static pmt_t s_response_current_capacity_allocation  = pmt_intern("response-current-capacity-allocation");


// ----------------------------------------------------------------------------------------------

class qa_alloc_top : public mb_mblock
{
  mb_port_sptr d_tx;
  mb_port_sptr d_rx;
  mb_port_sptr d_cs;

  long d_nmsgs_to_recv;
  long d_nrecvd;

  long d_max_capacity;
  long d_ntx_chan, d_nrx_chan;

  long d_nstatus;
  long d_nstatus_to_recv;

 public:
  qa_alloc_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~qa_alloc_top();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void check_message(mb_message_sptr msg);
  void run_tests();
};

qa_alloc_top::qa_alloc_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{ 
  d_nrecvd=0;
  d_nmsgs_to_recv = 7;
  d_nstatus=0;
  d_nstatus_to_recv = 3;
  
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);

  // Test the TX side
  define_component("server", "usrp_server", PMT_F);
  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");

}

qa_alloc_top::~qa_alloc_top(){}

void
qa_alloc_top::initial_transition()
{
  // Retrieve information about the USRP, then run tests
  d_cs->send(s_cmd_max_capacity, pmt_list1(PMT_F));
  d_cs->send(s_cmd_ntx_chan, pmt_list1(PMT_F));
  d_cs->send(s_cmd_nrx_chan, pmt_list1(PMT_F));
}

void
qa_alloc_top::run_tests()
{
  std::cout << "[qa_alloc_top] Starting tests...\n";
  // should be able to allocate 1 byte
  d_tx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));
  
  // should not be able to allocate max capacity after 100 bytes were allocated
  d_tx->send(s_cmd_allocate_channel, pmt_list2(pmt_from_long(usrp_server::RQSTD_CAPACITY_UNAVAIL), pmt_from_long(d_max_capacity)));  
  
  // keep allocating a little more until all of the channels are used and test the error response
  // we start at 1 since we've already allocated 1 channel
  for(int i=1; i < d_ntx_chan; i++) {
    d_tx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));
    d_nmsgs_to_recv++;
  }
  d_tx->send(s_cmd_allocate_channel, pmt_list2(pmt_from_long(usrp_server::CHANNEL_UNAVAIL), pmt_from_long(1)));

  // test out the same on the RX side
  d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));
  d_rx->send(s_cmd_allocate_channel, pmt_list2(pmt_from_long(usrp_server::RQSTD_CAPACITY_UNAVAIL), pmt_from_long(d_max_capacity)));  

  for(int i=1; i < d_nrx_chan; i++) {
    d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));
    d_nmsgs_to_recv++;
  }
  d_rx->send(s_cmd_allocate_channel, pmt_list2(pmt_from_long(usrp_server::CHANNEL_UNAVAIL), pmt_from_long(1)));

  // when all is said and done, there should be d_ntx_chan+d_ntx_chan bytes allocated
  d_cs->send(s_cmd_current_capacity_allocation, pmt_list1(pmt_from_long(d_ntx_chan+d_nrx_chan)));
}

void
qa_alloc_top::handle_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();

  if ((pmt_eq(msg->port_id(), d_tx->port_symbol())
       || pmt_eq(msg->port_id(), d_rx->port_symbol()))
       && pmt_eq(msg->signal(), s_response_allocate_channel))
    check_message(msg);
  
  if (pmt_eq(msg->port_id(), d_cs->port_symbol())) {
      
    if(pmt_eq(msg->signal(), s_response_max_capacity)) {
      d_max_capacity = pmt_to_long(pmt_nth(1, data));
      std::cout << "[qa_alloc_top] USRP has max capacity of " << d_max_capacity << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_ntx_chan)) {
      d_ntx_chan = pmt_to_long(pmt_nth(1, data));
      std::cout << "[qa_alloc_top] USRP tx channels: " << d_ntx_chan << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_nrx_chan)) {
      d_nrx_chan = pmt_to_long(pmt_nth(1, data));
      std::cout << "[qa_alloc_top] USRP rx channels: " << d_nrx_chan << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_current_capacity_allocation)) {
      check_message(msg);
    }
    
    d_nstatus++;

    if(d_nstatus==d_nstatus_to_recv)
      run_tests();
  }
}

void
qa_alloc_top::check_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();

  pmt_t expected_result = pmt_nth(0, data);
  pmt_t result = pmt_nth(1, data);
  
  d_nrecvd++;


  if(!pmt_eqv(expected_result, result)) {
    std::cout << "Got: " << result << " Expected: " << expected_result << "\n";
    shutdown_all(PMT_F);
  } else {
    std::cout << "[qa_alloc_top] Received expected response for message " << d_nrecvd << "\n";
  }

  if(d_nrecvd == d_nmsgs_to_recv)
    shutdown_all(PMT_T);
}

REGISTER_MBLOCK_CLASS(qa_alloc_top);

// ----------------------------------------------------------------------------------------------

class qa_dealloc_top : public mb_mblock
{
  mb_port_sptr d_tx;
  mb_port_sptr d_rx;
  mb_port_sptr d_cs;
  
  long d_max_capacity;
  long d_ntx_chan, d_nrx_chan;

  long d_nstatus;
  long d_nstatus_to_recv;

  long d_nalloc_to_recv;
  long d_nalloc_recvd;

  long d_ndealloc_to_recv;
  long d_ndealloc_recvd;

  std::vector<long> d_tx_chans;
  std::vector<long> d_rx_chans;

 public:
  qa_dealloc_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~qa_dealloc_top();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void check_allocation(mb_message_sptr msg);
  void check_deallocation(mb_message_sptr msg);
  void allocate_max();
  void deallocate_all();
};

qa_dealloc_top::qa_dealloc_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{ 
  d_ndealloc_recvd=0;
  d_ndealloc_to_recv = 0;
  d_nalloc_recvd=0;
  d_nalloc_to_recv = 0;
  d_nstatus=0;
  d_nstatus_to_recv = 3;
  
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);

  // Test the TX side
  define_component("server", "usrp_server", PMT_F);
  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");
}

qa_dealloc_top::~qa_dealloc_top(){}

void
qa_dealloc_top::initial_transition()
{
  // Retrieve information about the USRP, then run tests
  d_cs->send(s_cmd_max_capacity, pmt_list1(PMT_F));
  d_cs->send(s_cmd_ntx_chan, pmt_list1(PMT_F));
  d_cs->send(s_cmd_nrx_chan, pmt_list1(PMT_F));
}

void
qa_dealloc_top::allocate_max()
{
  std::cout << "[qa_dealloc_top] Max allocating...\n";

  // Keep allocating until we hit the maximum number of channels
  for(int i=0; i < d_ntx_chan; i++) {
    d_tx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));
    d_nalloc_to_recv++;
  }
  for(int i=0; i < d_nrx_chan; i++) {
    d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));
    d_nalloc_to_recv++;
  }
}

void
qa_dealloc_top::deallocate_all() {
  
  // Deallocate all of the channels that were allocated from allocate_max()
  for(int i=0; i < (int)d_tx_chans.size(); i++) {
    d_tx->send(s_cmd_deallocate_channel, pmt_list2(PMT_T, pmt_from_long(d_tx_chans[i])));
    d_ndealloc_to_recv++;
  }
  for(int i=0; i < (int)d_rx_chans.size(); i++) {
    d_rx->send(s_cmd_deallocate_channel, pmt_list2(PMT_T, pmt_from_long(d_rx_chans[i])));
    d_ndealloc_to_recv++;
  }

  // Should get permission denied errors trying to re-dealloc the channels, as we no
  // longer have permission to them after deallocating
  for(int i=0; i < (int)d_tx_chans.size(); i++) {
    d_tx->send(s_cmd_deallocate_channel, pmt_list2(pmt_from_long(usrp_server::PERMISSION_DENIED), pmt_from_long(d_tx_chans[i])));
    d_ndealloc_to_recv++;
  }
  for(int i=0; i < (int)d_rx_chans.size(); i++) {
    d_rx->send(s_cmd_deallocate_channel, pmt_list2(pmt_from_long(usrp_server::PERMISSION_DENIED), pmt_from_long(d_rx_chans[i])));
    d_ndealloc_to_recv++;
  }

  // Try to deallocate a channel that doesn't exist on both sides, the last element in the vectors
  // is the highest channel number, so we take that plus 1
  d_ndealloc_to_recv+=2;
  d_tx->send(s_cmd_deallocate_channel, pmt_list2(pmt_from_long(usrp_server::CHANNEL_INVALID), pmt_from_long(d_rx_chans.back()+1)));
  d_rx->send(s_cmd_deallocate_channel, pmt_list2(pmt_from_long(usrp_server::CHANNEL_INVALID), pmt_from_long(d_rx_chans.back()+1)));


  // The used capacity should be back to 0 now that we've deallocated everything
  d_cs->send(s_cmd_current_capacity_allocation, pmt_list1(pmt_from_long(0)));
}

void
qa_dealloc_top::handle_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  if (pmt_eq(msg->port_id(), d_tx->port_symbol())
       || pmt_eq(msg->port_id(), d_rx->port_symbol())) {
    
    if(pmt_eq(msg->signal(), s_response_allocate_channel)) {
      check_allocation(msg);
    }
    
    if(pmt_eq(msg->signal(), s_response_deallocate_channel)){
      check_deallocation(msg);
    }
  }
  
  if (pmt_eq(msg->port_id(), d_cs->port_symbol())) {
      
    if(pmt_eq(msg->signal(), s_response_max_capacity)) {
      d_max_capacity = pmt_to_long(pmt_nth(1, data));
      std::cout << "[qa_dealloc_top] USRP has max capacity of " << d_max_capacity << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_ntx_chan)) {
      d_ntx_chan = pmt_to_long(pmt_nth(1, data));
      std::cout << "[qa_dealloc_top] USRP tx channels: " << d_ntx_chan << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_nrx_chan)) {
      d_nrx_chan = pmt_to_long(pmt_nth(1, data));
      std::cout << "[qa_dealloc_top] USRP rx channels: " << d_nrx_chan << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_current_capacity_allocation)) {
      // the final command is a capacity check which should be 0, then we shutdown
      pmt_t expected_result = pmt_nth(0, data);
      pmt_t result = pmt_nth(1, data);

      if(pmt_eqv(expected_result, result))
        shutdown_all(PMT_T);
      else
        shutdown_all(PMT_F);
    }
    
    d_nstatus++;

    if(d_nstatus==d_nstatus_to_recv)
      allocate_max();
  }
}

void
qa_dealloc_top::check_deallocation(mb_message_sptr msg)
{
  pmt_t data = msg->data();

  pmt_t expected_result = pmt_nth(0, data);
  pmt_t result = pmt_nth(1, data);

  d_ndealloc_recvd++;

  if(!pmt_eqv(expected_result, result)) {
    std::cout << "Got: " << result << " Expected: " << expected_result << "\n";
    shutdown_all(PMT_F);
  } else {
    std::cout << "[qa_dealloc_top] Received expected deallocation response for message " << d_ndealloc_recvd << "\n";
  }
}

void
qa_dealloc_top::check_allocation(mb_message_sptr msg)
{
  pmt_t data = msg->data();

  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);
  pmt_t channel = pmt_nth(2, data);
  
  d_nalloc_recvd++;

  if(pmt_eqv(status, PMT_F)) {
    std::cout << "[qa_dealloc_top] Unexpected error response when allocating channels\n";
    shutdown_all(PMT_F);
  } else {
    // store all of the allocate channel numbers
    if(pmt_eq(msg->port_id(), d_tx->port_symbol()))
      d_tx_chans.push_back(pmt_to_long(channel));
    if(pmt_eq(msg->port_id(), d_rx->port_symbol()))
      d_rx_chans.push_back(pmt_to_long(channel));
  }

  if(d_nalloc_recvd == d_nalloc_to_recv) {
    
    std::cout << "[qa_dealloc_top] Allocated TX channels: ";
    for(int i=0; i < (int)d_tx_chans.size(); i++)
      std::cout << d_tx_chans[i] << " ";

    std::cout << "\n[qa_dealloc_top] Allocated RX channels: ";
    for(int i=0; i < (int)d_rx_chans.size(); i++)
      std::cout << d_rx_chans[i] << " ";
    std::cout << "\n";

    deallocate_all();   // once we've allocated all of our channels, try to dealloc them
  }
}

REGISTER_MBLOCK_CLASS(qa_dealloc_top);

// ----------------------------------------------------------------------------------------------

void 
qa_inband_usrp_server::test_chan_allocation()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  rt->run("top", "qa_alloc_top", PMT_F, &result);
  
  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

void
qa_inband_usrp_server::test_chan_deallocation()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  rt->run("top", "qa_dealloc_top", PMT_F, &result);
  
  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

void
qa_inband_usrp_server::test_fragmentation()
{
  
}
