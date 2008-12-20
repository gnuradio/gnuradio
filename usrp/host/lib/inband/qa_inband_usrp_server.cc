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

#include <usrp_inband_usb_packet.h>
#include <qa_inband_usrp_server.h>
#include <cppunit/TestAssert.h>
#include <stdio.h>
#include <usrp_server.h>
#include <mblock/mblock.h>
#include <mblock/runtime.h>
#include <mblock/protocol_class.h>
#include <mblock/class_registry.h>
#include <vector>
#include <iostream>
#include <pmt.h>

#include <symbols_usrp_server_cs.h>
#include <symbols_usrp_tx.h>
#include <symbols_usrp_rx.h>
#include <symbols_usrp_channel.h>
#include <symbols_usrp_low_level_cs.h>

typedef usrp_inband_usb_packet transport_pkt;   // makes conversion to gigabit easy

static bool verbose = false;

static pmt_t s_timeout = pmt_intern("%timeout");

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
  d_nmsgs_to_recv = 6;
  d_nstatus=0;
  d_nstatus_to_recv = 50;
  
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);
 
  // Use the stub with the usrp_server
  pmt_t usrp_server_dict = pmt_make_dict();
  pmt_dict_set(usrp_server_dict, pmt_intern("fake-usrp"),PMT_T);

  // Test the TX side
  define_component("server", "usrp_server", usrp_server_dict);
  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");

}

qa_alloc_top::~qa_alloc_top(){}

void
qa_alloc_top::initial_transition()
{
  // Allocations should fail before open
  d_tx->send(s_cmd_allocate_channel, 
             pmt_list2(pmt_list2(s_response_allocate_channel, 
                                 s_err_usrp_not_opened), 
                       pmt_from_long(1)));

  d_rx->send(s_cmd_allocate_channel, 
             pmt_list2(pmt_list2(s_response_allocate_channel,
                                 s_err_usrp_not_opened), 
                       pmt_from_long(1)));

  // Retrieve information about the USRP, then run tests
  d_cs->send(s_cmd_open, 
             pmt_list2(pmt_list2(s_response_open, PMT_T), 
             pmt_from_long(0)));

  d_cs->send(s_cmd_max_capacity, 
             pmt_list1(pmt_list2(s_response_max_capacity, PMT_T)));
  
  d_cs->send(s_cmd_ntx_chan, 
             pmt_list1(pmt_list2(s_response_ntx_chan, PMT_T)));
  
  d_cs->send(s_cmd_nrx_chan, 
             pmt_list1(pmt_list2(s_response_nrx_chan,PMT_T)));
}

void
qa_alloc_top::run_tests()
{
  if(verbose)
    std::cout << "[qa_alloc_top] Starting tests...\n";

  // should be able to allocate 1 byte
  d_tx->send(s_cmd_allocate_channel, 
             pmt_list2(PMT_T, pmt_from_long(1)));
  
  // should not be able to allocate max capacity after 100 bytes were allocated
  d_tx->send(s_cmd_allocate_channel, 
             pmt_list2(s_err_requested_capacity_unavailable, 
                       pmt_from_long(d_max_capacity)));  
  
  // keep allocating a little more until all of the channels are used and test
  // the error response we start at 1 since we've already allocated 1 channel
  for(int i=1; i < d_ntx_chan; i++) {

    if(verbose)
      std::cout << "[qa_alloc_top] Sent allocation request...\n";
  
    d_tx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));

    d_nmsgs_to_recv++;
  }

  // No more channels after allocating all of them is expected
  d_tx->send(s_cmd_allocate_channel, 
             pmt_list2(s_err_channel_unavailable, 
                       pmt_from_long(1)));

  // test out the same on the RX side
  d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));

  d_rx->send(s_cmd_allocate_channel, 
             pmt_list2(s_err_requested_capacity_unavailable, 
                       pmt_from_long(d_max_capacity)));  

  for(int i=1; i < d_nrx_chan; i++) {
    
    d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));
    
    d_nmsgs_to_recv++;
  }

  d_rx->send(s_cmd_allocate_channel, 
             pmt_list2(s_err_channel_unavailable, 
             pmt_from_long(1)));

  // when all is said and done, there should be d_ntx_chan+d_ntx_chan bytes
  // allocated
  d_cs->send(s_cmd_current_capacity_allocation, 
             pmt_list1(pmt_from_long(d_ntx_chan+d_nrx_chan)));
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
      d_max_capacity = pmt_to_long(pmt_nth(2, data));
      if(verbose)
        std::cout << "[qa_alloc_top] USRP has max capacity of " 
                  << d_max_capacity << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_ntx_chan)) {
      d_ntx_chan = pmt_to_long(pmt_nth(2, data));
      if(verbose)
        std::cout << "[qa_alloc_top] USRP tx channels: " 
                  << d_ntx_chan << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_nrx_chan)) {
      d_nrx_chan = pmt_to_long(pmt_nth(2, data));
      if(verbose)
        std::cout << "[qa_alloc_top] USRP rx channels: " 
                  << d_nrx_chan << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_current_capacity_allocation)) {
      check_message(msg);
    }
    
    d_nstatus++;

    check_message(msg);

    if(d_nstatus==d_nstatus_to_recv)
      run_tests();
  }
}

void
qa_alloc_top::check_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();

  pmt_t expected = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);

  pmt_t e_event = pmt_nth(0, expected);
  pmt_t e_status = pmt_nth(1, expected);
  
  d_nrecvd++;


  if(!pmt_eqv(e_status, status) || !pmt_eqv(e_event, event)) {
    if(verbose)
      std::cout << "Got: " << status << " Expected: " << e_status << "\n";
    shutdown_all(PMT_F);
    return;
  } else {
    if(verbose)
      std::cout << "[qa_alloc_top] Received expected response for message " 
                << d_nrecvd << " (" << event << ")\n";
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
  d_nalloc_to_recv = 0;   // auto-set
  d_nstatus=0;
  d_nstatus_to_recv = 4;
  
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);

  // Use the stub with the usrp_server
  pmt_t usrp_server_dict = pmt_make_dict();
  pmt_dict_set(usrp_server_dict, pmt_intern("fake-usrp"),PMT_T);

  // Test the TX side
  define_component("server", "usrp_server", usrp_server_dict);
  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");
}

qa_dealloc_top::~qa_dealloc_top(){}

void
qa_dealloc_top::initial_transition()
{

  if(verbose)
    std::cout << "[qa_dealloc_top] Initializing...\n";

  // Retrieve information about the USRP, then run tests
  d_cs->send(s_cmd_open, 
             pmt_list2(pmt_list2(s_response_open,PMT_T), 
             pmt_from_long(0)));

  d_cs->send(s_cmd_max_capacity, 
             pmt_list1(pmt_list2(s_response_max_capacity,PMT_T)));

  d_cs->send(s_cmd_ntx_chan, 
             pmt_list1(pmt_list2(s_response_ntx_chan,PMT_T)));

  d_cs->send(s_cmd_nrx_chan, 
             pmt_list1(pmt_list2(s_response_nrx_chan,PMT_T)));
}

void
qa_dealloc_top::allocate_max()
{

  // Keep allocating until we hit the maximum number of channels
  for(int i=0; i < d_ntx_chan; i++) {
    d_tx->send(s_cmd_allocate_channel, 
               pmt_list2(pmt_list2(s_response_allocate_channel,PMT_T),
               pmt_from_long(1)));  // 1 byte is good enough

    d_nalloc_to_recv++;
  }

  for(int i=0; i < d_nrx_chan; i++) {
    d_rx->send(s_cmd_allocate_channel, 
               pmt_list2(pmt_list2(s_response_allocate_channel,PMT_T), 
               pmt_from_long(1)));

    d_nalloc_to_recv++;
  }

}

void
qa_dealloc_top::deallocate_all() {
  
  // Deallocate all of the channels that were allocated from allocate_max()
  for(int i=0; i < (int)d_tx_chans.size(); i++) {

    if(verbose)
      std::cout << "[qa_dealloc_top] Trying to dealloc TX " 
                << d_tx_chans[i] << std::endl;

    d_tx->send(s_cmd_deallocate_channel, 
               pmt_list2(pmt_list2(s_response_deallocate_channel,PMT_T), 
               pmt_from_long(d_tx_chans[i])));

    d_ndealloc_to_recv++;
  }

  // Deallocate the RX side now
  for(int i=0; i < (int)d_rx_chans.size(); i++) {

    if(verbose)
      std::cout << "[qa_dealloc_top] Trying to dealloc RX " 
                << d_tx_chans[i] << std::endl;

    d_rx->send(s_cmd_deallocate_channel, 
               pmt_list2(pmt_list2(s_response_deallocate_channel,PMT_T), 
               pmt_from_long(d_rx_chans[i])));

    d_ndealloc_to_recv++;
  }

  // Should get permission denied errors trying to re-dealloc the channels, as
  // we no longer have permission to them after deallocating
  for(int i=0; i < (int)d_tx_chans.size(); i++) {

    d_tx->send(s_cmd_deallocate_channel, 
               pmt_list2(pmt_list2(s_response_deallocate_channel,
                             s_err_channel_permission_denied), 
                         pmt_from_long(d_tx_chans[i])));

    d_ndealloc_to_recv++;
  }

  // Same for RX
  for(int i=0; i < (int)d_rx_chans.size(); i++) {

    d_rx->send(s_cmd_deallocate_channel, 
               pmt_list2(pmt_list2(s_response_deallocate_channel,
                             s_err_channel_permission_denied), 
                         pmt_from_long(d_rx_chans[i])));
  
    d_ndealloc_to_recv++;
  }

  // Try to deallocate a channel that doesn't exist on both sides, the last
  // element in the vectors is the highest channel number, so we take that plus
  // 1
  d_ndealloc_to_recv+=2;
  d_tx->send(s_cmd_deallocate_channel, 
             pmt_list2(pmt_list2(s_response_deallocate_channel,
                                 s_err_channel_invalid), 
                       pmt_from_long(d_rx_chans.back()+1)));

  d_rx->send(s_cmd_deallocate_channel, 
             pmt_list2(pmt_list2(s_response_deallocate_channel,
                                 s_err_channel_invalid), 
                       pmt_from_long(d_rx_chans.back()+1)));


  // The used capacity should be back to 0 now that we've deallocated everything
  d_cs->send(s_cmd_current_capacity_allocation,
             pmt_list1(pmt_list2(s_response_current_capacity_allocation,
                                 PMT_T)));
}

void
qa_dealloc_top::handle_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();
  
  if(pmt_eq(event, pmt_intern("%shutdown")))
    return;

  pmt_t expected = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);

  pmt_t e_event = pmt_nth(0, expected);
  pmt_t e_status = pmt_nth(1, expected);

  if(!pmt_eqv(e_status, status) || !pmt_eqv(e_event, event)) {
    if(verbose)
      std::cout << "Got: " << status << " Expected: " << e_status << "\n";
    shutdown_all(PMT_F);
    return;
  } else {
    if(verbose)
      std::cout << "[qa_alloc_top] Received expected response for message " 
                << d_ndealloc_recvd
      << " (" << event << ")\n";
  }

  if (pmt_eq(msg->port_id(), d_tx->port_symbol())
       || pmt_eq(msg->port_id(), d_rx->port_symbol())) {
    
    if(pmt_eq(msg->signal(), s_response_allocate_channel)) {
      check_allocation(msg);
    }
    
  }
  
  if (pmt_eq(msg->port_id(), d_cs->port_symbol())) {
      
    if(pmt_eq(msg->signal(), s_response_max_capacity)) {
      d_max_capacity = pmt_to_long(pmt_nth(2, data));
    }
    else if(pmt_eq(msg->signal(), s_response_ntx_chan)) {
      d_ntx_chan = pmt_to_long(pmt_nth(2, data));
    }
    else if(pmt_eq(msg->signal(), s_response_nrx_chan)) {
      d_nrx_chan = pmt_to_long(pmt_nth(2, data));
    }
    else if(pmt_eq(msg->signal(), s_response_current_capacity_allocation)) {
      // the final command is a capacity check which should be 0, then we
      // shutdown
      pmt_t expected_result = pmt_from_long(0);
      pmt_t result = pmt_nth(2, data);

      if(pmt_eqv(expected_result, result)) {
        shutdown_all(PMT_T);
        return;
      } else {
        shutdown_all(PMT_F);
        return;
      }
    }
    
    d_nstatus++;

    if(d_nstatus==d_nstatus_to_recv)
      allocate_max();
  }
}


void
qa_dealloc_top::check_allocation(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();

  pmt_t expected = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);
  pmt_t channel = pmt_nth(2, data);

  d_nalloc_recvd++;

  if(!pmt_eqv(status, PMT_T)) {
    shutdown_all(PMT_F);
    return;
  } else {
    // store all of the allocate channel numbers
    if(pmt_eq(msg->port_id(), d_tx->port_symbol()))
      d_tx_chans.push_back(pmt_to_long(channel));
    if(pmt_eq(msg->port_id(), d_rx->port_symbol()))
      d_rx_chans.push_back(pmt_to_long(channel));
  }

  if(d_nalloc_recvd == d_nalloc_to_recv) {

    if(verbose) {
      std::cout << "[qa_dealloc_top] Allocated TX channels: ";
      for(int i=0; i < (int)d_tx_chans.size(); i++)
        std::cout << d_tx_chans[i] << " ";

      std::cout << "\n[qa_dealloc_top] Allocated RX channels: ";
      for(int i=0; i < (int)d_rx_chans.size(); i++)
        std::cout << d_rx_chans[i] << " ";
      std::cout << "\n";
    }

    deallocate_all();   // once we've allocated all of our channels, try to
                        // dealloc them
  }
}

REGISTER_MBLOCK_CLASS(qa_dealloc_top);

// ----------------------------------------------------------------------------------------------

class qa_open_close_top : public mb_mblock
{
  mb_port_sptr d_cs;
  
  long d_max_capacity;

  long d_nmsg_to_recv;
  long d_nmsg_recvd;

 public:
  qa_open_close_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~qa_open_close_top();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void check_cs(mb_message_sptr msg);
  void run_tests();
};

qa_open_close_top::qa_open_close_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{ 

  d_nmsg_to_recv=7;
  d_nmsg_recvd=0;
  
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);

  // Use the stub with the usrp_server
  pmt_t usrp_server_dict = pmt_make_dict();
  pmt_dict_set(usrp_server_dict, pmt_intern("fake-usrp"),PMT_T);

  // Test the TX side
  define_component("server", "usrp_server", usrp_server_dict);
  connect("self", "cs", "server", "cs");
}

qa_open_close_top::~qa_open_close_top(){}

void
qa_open_close_top::initial_transition()
{
  run_tests();
}

void
qa_open_close_top::run_tests()
{
  // std::cout << "[qa_open_close_top] Starting tests\n";

  // A close before an open should fail
  d_cs->send(s_cmd_close, pmt_list1(pmt_list2(s_response_close, 
                                              s_err_usrp_already_closed)));
  
  // Perform an open, and a second open which should fail
  d_cs->send(s_cmd_open, 
             pmt_list2(pmt_list2(s_response_open,PMT_T), 
                       pmt_from_long(0)));

  d_cs->send(s_cmd_open, 
             pmt_list2(pmt_list2(s_response_open,
                                 s_err_usrp_already_opened), 
                       pmt_from_long(0)));

  // A close should now be successful since the interface is open
  d_cs->send(s_cmd_close, pmt_list1(pmt_list2(s_response_close,PMT_T)));

  // But, a second close should fail
  d_cs->send(s_cmd_close, pmt_list1(pmt_list2(s_response_close,
                                              s_err_usrp_already_closed)));
  
  // Just to be thorough, try an open and close again
  d_cs->send(s_cmd_open, 
             pmt_list2(pmt_list2(s_response_open,PMT_T), 
                       pmt_from_long(0)));

  d_cs->send(s_cmd_close, pmt_list1(pmt_list2(s_response_close,PMT_T)));
  
}


void
qa_open_close_top::handle_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();

  if (pmt_eq(msg->port_id(), d_cs->port_symbol())) {
      check_cs(msg);
  }

  d_nmsg_recvd++;

  if(d_nmsg_to_recv == d_nmsg_recvd)
    shutdown_all(PMT_T);
}

void
qa_open_close_top::check_cs(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();

  pmt_t expected = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);

  pmt_t e_event = pmt_nth(0, expected);
  pmt_t e_status = pmt_nth(1, expected);

  if(!pmt_eqv(e_status, status) || !pmt_eqv(e_event, event)) {

    if(verbose)
      std::cout << "[qa_open_close_top] FAILED check_cs... Got: " << status
                << " Expected: " << e_status
                << " for event " << event << "\n";

    shutdown_all(PMT_F);
  } else {
    if(verbose)
      std::cout << "[qa_open_close_top] Received expected CS response (" 
                << event << ")\n";
  }

}

REGISTER_MBLOCK_CLASS(qa_open_close_top);

// ----------------------------------------------------------------------------------------------

class qa_tx_top : public mb_mblock
{
  mb_port_sptr d_tx;
  mb_port_sptr d_rx;
  mb_port_sptr d_cs;
  
  long d_max_capacity;
  long d_ntx_chan, d_nrx_chan;

  long d_tx_chan;
  long d_rx_chan;

  long d_nmsg_to_recv;
  long d_nmsg_recvd;

 public:
  qa_tx_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~qa_tx_top();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void check_allocation(mb_message_sptr msg);
  void check_deallocation(mb_message_sptr msg);
  void check_xmit(mb_message_sptr msg);
  void check_cs(mb_message_sptr msg);
  void run_tests();
};

qa_tx_top::qa_tx_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{ 

  d_nmsg_to_recv=10;
  d_nmsg_recvd=0;
  
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);

  // Use the stub with the usrp_server
  pmt_t usrp_server_dict = pmt_make_dict();
  pmt_dict_set(usrp_server_dict, pmt_intern("fake-usrp"),PMT_T);

  // Test the TX side
  define_component("server", "usrp_server", usrp_server_dict);
  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");
}

qa_tx_top::~qa_tx_top(){}

void
qa_tx_top::initial_transition()
{
  run_tests();
}

void
qa_tx_top::run_tests()
{
  if(verbose)
   std::cout << "[qa_tx_top] Starting tests\n";

  // A transmit before an open should fail
  d_tx->send(s_cmd_xmit_raw_frame, 
             pmt_list4(pmt_list2(s_response_xmit_raw_frame, 
                                 s_err_usrp_not_opened), 
                       pmt_from_long(0), 
                       pmt_make_u32vector(transport_pkt::max_payload()/4, 0),
                       pmt_from_long(0)));
  
  // Now open
  d_cs->send(s_cmd_open, 
             pmt_list2(pmt_list2(s_response_open,PMT_T), 
                       pmt_from_long(0)));

  // Try to transmit on a channel that we have no allocation for
  d_tx->send(s_cmd_xmit_raw_frame, 
             pmt_list4(pmt_list2(s_response_xmit_raw_frame,
                                 s_err_channel_permission_denied), 
                       pmt_from_long(0), 
                       pmt_make_u32vector(transport_pkt::max_payload()/4, 0), 
                       pmt_from_long(0)));

  // Get a channel allocation and send on it, we assume 0 (FIXME) until 'defer'
  // is implemented for simplicity
  d_tx->send(s_cmd_allocate_channel, 
             pmt_list2(pmt_list2(s_response_allocate_channel, PMT_T), 
                       pmt_from_long(1)));

  d_tx->send(s_cmd_xmit_raw_frame, 
             pmt_list4(pmt_list2(s_response_xmit_raw_frame, PMT_T), 
                       pmt_from_long(0), 
                       pmt_make_u32vector(transport_pkt::max_payload()/4, 0), 
                       pmt_from_long(0)));

  // Close should be successful
  d_cs->send(s_cmd_close, pmt_list1(pmt_list2(s_response_close,PMT_T)));

  // After closing, a new transmit raw frame should fail again
  d_tx->send(s_cmd_xmit_raw_frame, 
             pmt_list4(pmt_list2(s_response_xmit_raw_frame, 
                                 s_err_usrp_not_opened), 
                       pmt_from_long(0), 
                       pmt_make_u32vector(transport_pkt::max_payload()/4, 0), 
                       pmt_from_long(0)));

  // Reopen and retry before getting an allocation, the first xmit should fail,
  // after we allocate it should work again
  d_cs->send(s_cmd_open, 
             pmt_list2(pmt_list2(s_response_open, PMT_T), 
                       pmt_from_long(0)));

  d_tx->send(s_cmd_xmit_raw_frame, 
             pmt_list4(pmt_list2(s_response_xmit_raw_frame,
                                 s_err_channel_permission_denied), 
                       pmt_from_long(0), 
                       pmt_make_u32vector(transport_pkt::max_payload()/4, 0), 
                       pmt_from_long(0)));

  d_tx->send(s_cmd_allocate_channel, 
             pmt_list2(pmt_list2(s_response_allocate_channel, PMT_T), 
                       pmt_from_long(1)));

  d_tx->send(s_cmd_xmit_raw_frame, 
             pmt_list4(pmt_list2(s_response_xmit_raw_frame,PMT_T), 
                       pmt_from_long(0), 
                       pmt_make_u32vector(transport_pkt::max_payload()/4, 0), 
                       pmt_from_long(0)));

  // A final close which should be successful
  d_cs->send(s_cmd_close, pmt_list1(pmt_list2(s_response_close,PMT_T)));
  
}


void
qa_tx_top::handle_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();
  
  if(pmt_eq(event, pmt_intern("%shutdown")))
    return;

  pmt_t expected = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);

  pmt_t e_event = pmt_nth(0, expected);
  pmt_t e_status = pmt_nth(1, expected);

  if(!pmt_eqv(e_status, status) || !pmt_eqv(e_event, event)) {
    if(verbose)
      std::cout << "[qa_xmit_top] Got: " << status 
                << " Expected: " << e_status 
                << "For signal: " << event << "\n";
    shutdown_all(PMT_F);
    return;
  } else {
    if(verbose)
      std::cout << "[qa_xmit_top] Received expected response for message " 
                << d_nmsg_recvd
      << " (" << event << ")\n";
  }

  if (pmt_eq(msg->port_id(), d_tx->port_symbol())
       || pmt_eq(msg->port_id(), d_rx->port_symbol())) {
    
    if(pmt_eq(msg->signal(), s_response_allocate_channel)) 
      check_allocation(msg);
    
  }
  
  d_nmsg_recvd++;

  if(d_nmsg_to_recv == d_nmsg_recvd){
    shutdown_all(PMT_T);
    return;
  }
}

void
qa_tx_top::check_allocation(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();
  
  pmt_t expected = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);
  pmt_t channel = pmt_nth(2, data);

  if(pmt_eqv(status, PMT_T)) {
    // store all of the allocate channel numbers
    if(pmt_eq(msg->port_id(), d_tx->port_symbol()))
      d_tx_chan = pmt_to_long(channel);
    if(pmt_eq(msg->port_id(), d_rx->port_symbol()))
      d_rx_chan = pmt_to_long(channel);
  }
}

REGISTER_MBLOCK_CLASS(qa_tx_top);

// ----------------------------------------------------------------------------------------------

class qa_rx_top : public mb_mblock
{
  mb_port_sptr d_rx;
  mb_port_sptr d_cs;
  
  long d_max_capacity;
  long d_ntx_chan, d_nrx_chan;

  long d_rx_chan;

  bool d_got_response_recv;

  mb_time d_t0;
  double d_delta_t;

 public:
  qa_rx_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~qa_rx_top();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void check_allocation(mb_message_sptr msg);
  void check_deallocation(mb_message_sptr msg);
  void check_xmit(mb_message_sptr msg);
  void check_cs(mb_message_sptr msg);
  void run_tests();
};

qa_rx_top::qa_rx_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
    d_got_response_recv(false)
{ 

  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);

  // Use the stub with the usrp_server
  pmt_t usrp_dict = pmt_make_dict();
  // Set TX and RX interpolations
  pmt_dict_set(usrp_dict,
               pmt_intern("decim-rx"),
               pmt_from_long(128));
  pmt_dict_set(usrp_dict, pmt_intern("fake-usrp"), PMT_T);

  // Test the TX side
  define_component("server", "usrp_server", usrp_dict);
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");
}

qa_rx_top::~qa_rx_top(){}

void
qa_rx_top::initial_transition()
{
  run_tests();
}

void
qa_rx_top::run_tests()
{
  if(verbose)
    std::cout << "[qa_rx_top] Starting tests\n";

  d_cs->send(s_cmd_open, pmt_list2(pmt_list2(s_response_open,PMT_T), pmt_from_long(0)));

  d_rx->send(s_cmd_allocate_channel, 
             pmt_list2(pmt_list2(s_response_allocate_channel,PMT_T), 
             pmt_from_long(1)));

  d_rx->send(s_cmd_start_recv_raw_samples, 
             pmt_list2(PMT_NIL, 
                       pmt_from_long(0)));

  // Schedule a small timeout in which we expect to have received at least one
  // packet worth of samples from the stub
  d_t0 = mb_time::time();
  schedule_one_shot_timeout(d_t0 + 0.01, PMT_NIL);
}


void
qa_rx_top::handle_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();
  
  if(pmt_eq(event, pmt_intern("%shutdown")))
    return;
  
  pmt_t expected = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);

  // If we get a timeout we shutdown
  if(pmt_eq(event, s_timeout)) {
    if(verbose)
      std::cout << "[qa_rx_top] Got timeout\n";
    d_rx->send(s_cmd_stop_recv_raw_samples, 
               pmt_list2(PMT_NIL, 
                         pmt_from_long(0)));

    d_cs->send(s_cmd_close, pmt_list1(pmt_list2(s_response_close,PMT_T)));
    return;
  }
  
  // For testing RX, an invocation handle is not generated by the stub,
  // therefore the same approach for testing is not used.  We simply
  // expect all responses to be true.
  if(pmt_eq(event, s_response_recv_raw_samples)) {
    if(pmt_eqv(status, PMT_T)) {

      if(verbose)
        std::cout << "[qa_rx_top] Received expected response for message " 
                  << " (" << event << ")\n";

      // All we want is 1 response receive!  Can't guarantee exact numbers
      d_got_response_recv = true;
    }
    else {
      if(verbose)
        std::cout << "Got: " << status << " Expected: " << PMT_T << "\n";
      shutdown_all(PMT_F);
    }
    return;
  }

  pmt_t e_event = pmt_nth(0, expected);
  pmt_t e_status = pmt_nth(1, expected);

  if(!pmt_eqv(e_status, status) || !pmt_eqv(e_event, event)) {
   if(verbose)
      std::cout << "Got: " << status << " Expected: " << e_status << "\n";
    shutdown_all(PMT_F);
    return;
  } else {
    if(verbose)
      std::cout << "[qa_rx_top] Received expected response for message " 
                << " (" << event << ")\n";
  }

  if (pmt_eq(msg->port_id(), d_rx->port_symbol())) {
    
    if(pmt_eq(msg->signal(), s_response_allocate_channel)) 
      check_allocation(msg);

  }

  // We stop when we get a close, we are successful if we
  // got a response from recv, fail if we never got a recv response
  if(pmt_eq(msg->signal(), s_response_close)) {
    
    if(d_got_response_recv) {
      shutdown_all(PMT_T);
      return;
    }
    else {
      shutdown_all(PMT_F);
      if(verbose)
        std::cout << "[qa_rx_top] No response message before close\n";
      return;
    }
  }
}


void
qa_rx_top::check_allocation(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();
  
  pmt_t expected = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);
  pmt_t channel = pmt_nth(2, data);

  if(pmt_eqv(status, PMT_T)) {
    // store all of the allocate channel numbers
    if(pmt_eq(msg->port_id(), d_rx->port_symbol()))
      d_rx_chan = pmt_to_long(channel);
  }
}

REGISTER_MBLOCK_CLASS(qa_rx_top);

// ----------------------------------------------------------------------------------------------

class qa_rid_top : public mb_mblock
{
  mb_port_sptr d_tx;
  mb_port_sptr d_rx;
  mb_port_sptr d_cs;

  long d_npongs;
  long d_tcycles;
  long d_cycles;
  long d_max_rid;
  
  mb_time d_t0;
  double d_delta_t;

 public:
  qa_rid_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~qa_rid_top();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void run_tests();
  void send_max_pings();
};

qa_rid_top::qa_rid_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{ 
  d_npongs = 0;
  d_tcycles = 3;
  d_cycles = d_tcycles;
  d_max_rid = usrp_server::D_MAX_RID;
  d_delta_t = 0.1;


  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);
 
  // Use the stub with the usrp_server
  pmt_t usrp_server_dict = pmt_make_dict();
  pmt_dict_set(usrp_server_dict, pmt_intern("fake-usrp"),PMT_T);

  // Test the TX side
  define_component("server", "usrp_server", usrp_server_dict);
  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");

}

qa_rid_top::~qa_rid_top(){}

void
qa_rid_top::initial_transition()
{
  run_tests();
}

void
qa_rid_top::run_tests()
{
  if(verbose)
    std::cout << "[qa_rid_top] Starting tests...\n";
  
  // Retrieve information about the USRP, then run tests
  d_cs->send(s_cmd_open, 
             pmt_list2(pmt_list2(s_response_open, PMT_T), 
             pmt_from_long(0)));

  // should be able to allocate 1 byte
  d_tx->send(s_cmd_allocate_channel, 
             pmt_list2(pmt_list2(s_response_allocate_channel, PMT_T), 
                       pmt_from_long(1)));
  
  d_rx->send(s_cmd_allocate_channel, 
             pmt_list2(pmt_list2(s_response_allocate_channel, PMT_T), 
                       pmt_from_long(1)));
  
  // Need to start receiving to read from the USRP to get C/S responses
  d_rx->send(s_cmd_start_recv_raw_samples, 
             pmt_list2(PMT_NIL, 
                       pmt_from_long(0)));

  // Build a subpacket of MAX_RID pings and wait a small amount for all of the
  // responses and fire off another MAX_RID.  If MAX_RID*2 responses are
  // received, the RID recycling is working correctly.
  // Schedule a timer in which we expect to have received all of the responses,
  // which will send off another MAX_RID worth.
  send_max_pings();
  d_t0 = mb_time::time();
  schedule_one_shot_timeout(d_t0 + d_delta_t, PMT_NIL);
}

void
qa_rid_top::send_max_pings()
{
  pmt_t ping = pmt_list2(s_op_ping_fixed,
                         pmt_list2(pmt_from_long(0),
                                   pmt_from_long(0)));

  pmt_t sub_packets = PMT_NIL;

  for(int i=0; i<d_max_rid; i++) 
    sub_packets = pmt_list_add(sub_packets, ping);

  d_tx->send(s_cmd_to_control_channel,
             pmt_list2(pmt_list2(s_response_from_control_channel, PMT_T),
                       sub_packets));
}

void
qa_rid_top::handle_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();

  // If we get a timeout we ensure we got a maximum RID number of responses.
  if(pmt_eq(event, s_timeout)) {
    if(verbose)
      std::cout << "[qa_rid_top] Got timeout, received so far: " 
                << d_npongs << "\n";

    d_cycles--;
    
    if(d_cycles==0 && d_npongs == d_max_rid*d_tcycles) {
      shutdown_all(PMT_T);
    }
    else if(d_cycles==0) {

      std::cout << "[qa_rid_top] d_npongs: " << d_npongs
                << " expected: " << d_max_rid*d_tcycles
                << std::endl;

      shutdown_all(PMT_F);
    }
    else {
      send_max_pings();
      d_t0 = mb_time::time();
      schedule_one_shot_timeout(d_t0 + d_delta_t, PMT_NIL);
    }

  }
  else if(pmt_eq(event, s_response_from_control_channel))
  {
    d_npongs++;
  }

}

REGISTER_MBLOCK_CLASS(qa_rid_top);


// ----------------------------------------------------------------------------------------------

class qa_cs_top : public mb_mblock
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
  qa_cs_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~qa_cs_top();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void check_message(mb_message_sptr msg);
  void run_tests();
};

qa_cs_top::qa_cs_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{ 
  d_nrecvd=0;
  d_nmsgs_to_recv = 8;
  d_nstatus=0;
  d_nstatus_to_recv = 50;
  
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);
 
  // Use the stub with the usrp_server
  pmt_t usrp_server_dict = pmt_make_dict();
  pmt_dict_set(usrp_server_dict, pmt_intern("fake-usrp"),PMT_T);

  // Test the TX side
  define_component("server", "usrp_server", usrp_server_dict);
  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");

}

qa_cs_top::~qa_cs_top(){}

void
qa_cs_top::initial_transition()
{
  run_tests();
}

void
qa_cs_top::run_tests()
{
  if(verbose)
    std::cout << "[qa_cs_top] Starting tests...\n";
  
  // Retrieve information about the USRP, then run tests
  d_cs->send(s_cmd_open, 
             pmt_list2(pmt_list2(s_response_open, PMT_T), 
             pmt_from_long(0)));

  // should be able to allocate 1 byte
  d_tx->send(s_cmd_allocate_channel, 
             pmt_list2(pmt_list2(s_response_allocate_channel, PMT_T), 
                       pmt_from_long(1)));
  
  d_rx->send(s_cmd_allocate_channel, 
             pmt_list2(pmt_list2(s_response_allocate_channel, PMT_T), 
                       pmt_from_long(1)));
  
  // Need to start receiving to read from the USRP to get C/S responses
  d_rx->send(s_cmd_start_recv_raw_samples, 
             pmt_list2(PMT_NIL, 
                       pmt_from_long(0)));

  d_tx->send(s_cmd_to_control_channel,
             pmt_list2(pmt_list2(s_response_from_control_channel, PMT_T),
                       pmt_list1(
                            pmt_list2(s_op_ping_fixed, 
                                      pmt_list2(pmt_from_long(3), 
                                      pmt_from_long(0))))));
  
  d_tx->send(s_cmd_to_control_channel,
             pmt_list2(pmt_list2(s_response_from_control_channel, PMT_T),
                       pmt_list1(
                            pmt_list2(s_op_write_reg, 
                                      pmt_list2(
                                      pmt_from_long(0x3), 
                                      pmt_from_long(0x4))))));
  
  d_tx->send(s_cmd_to_control_channel,
             pmt_list2(pmt_list2(s_response_from_control_channel, PMT_T),
                       pmt_list1(
                            pmt_list2(s_op_write_reg_masked, 
                                      pmt_list3(
                                      pmt_from_long(0x3), 
                                      pmt_from_long(0x4),
                                      pmt_from_long(0x5))))));
  
  d_tx->send(s_cmd_to_control_channel,
             pmt_list2(pmt_list2(s_response_from_control_channel, PMT_T),
                       pmt_list1(
                            pmt_list2(s_op_read_reg, 
                                      pmt_list2(pmt_from_long(0), 
                                      pmt_from_long(0x6))))));
  
  d_tx->send(s_cmd_to_control_channel,
             pmt_list2(pmt_list2(s_response_from_control_channel, PMT_T),
                       pmt_list1(
                            pmt_list2(s_op_delay, 
                                      pmt_list1(pmt_from_long(0x7))))));

  pmt_t subpackets = pmt_list5(
                        pmt_list2(s_op_ping_fixed, pmt_list2(pmt_from_long(0), pmt_from_long(0))),
                        pmt_list2(s_op_delay, pmt_list1(pmt_from_long(0x7))),
                        pmt_list2(s_op_write_reg_masked, pmt_list3(pmt_from_long(3),
                                                                   pmt_from_long(4),
                                                                   pmt_from_long(5))),
                        pmt_list2(s_op_write_reg, pmt_list2(pmt_from_long(3),
                                                            pmt_from_long(4))),
                        pmt_list2(s_op_read_reg, pmt_list2(pmt_from_long(0),
                                                           pmt_from_long(6)))
                     );

  d_tx->send(s_cmd_to_control_channel, 
              pmt_list2(pmt_list2(s_response_from_control_channel, PMT_T),
                        subpackets));

  pmt_t i2c_data = pmt_make_u8vector(8, 0xff);

  subpackets = pmt_list2(
                        pmt_list2(s_op_i2c_write, 
                                  pmt_list2(pmt_from_long(8), i2c_data)),
                        pmt_list2(s_op_i2c_read,
                                  pmt_list3(pmt_from_long(0), pmt_from_long(9), pmt_from_long(1)))

                     );

  d_tx->send(s_cmd_to_control_channel, 
              pmt_list2(pmt_list2(s_response_from_control_channel, PMT_T),
                        subpackets));
  
}

void
qa_cs_top::handle_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();

  if ((pmt_eq(msg->port_id(), d_tx->port_symbol())
       || pmt_eq(msg->port_id(), d_rx->port_symbol()))
       && pmt_eq(msg->signal(), s_response_allocate_channel))
    check_message(msg);

  if (pmt_eq(msg->port_id(), d_tx->port_symbol())
      && pmt_eq(msg->signal(), s_response_from_control_channel))
    check_message(msg);
  
  if (pmt_eq(msg->port_id(), d_cs->port_symbol())) {
      
    if(pmt_eq(msg->signal(), s_response_max_capacity)) {
      d_max_capacity = pmt_to_long(pmt_nth(2, data));
      if(verbose)
        std::cout << "[qa_cs_top] USRP has max capacity of " 
                  << d_max_capacity << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_ntx_chan)) {
      d_ntx_chan = pmt_to_long(pmt_nth(2, data));
      if(verbose)
        std::cout << "[qa_cs_top] USRP tx channels: " 
                  << d_ntx_chan << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_nrx_chan)) {
      d_nrx_chan = pmt_to_long(pmt_nth(2, data));
      if(verbose)
        std::cout << "[qa_cs_top] USRP rx channels: " 
                  << d_nrx_chan << "\n";
    }
    else if(pmt_eq(msg->signal(), s_response_current_capacity_allocation)) {
      check_message(msg);
    }
    
    d_nstatus++;

    check_message(msg);

    if(d_nstatus==d_nstatus_to_recv)
      run_tests();
  }
}

void
qa_cs_top::check_message(mb_message_sptr msg)
{
  pmt_t data = msg->data();
  pmt_t event = msg->signal();

  pmt_t expected = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);

  pmt_t e_event = pmt_nth(0, expected);
  pmt_t e_status = pmt_nth(1, expected);
  
  d_nrecvd++;


  if(!pmt_eqv(e_status, status) || !pmt_eqv(e_event, event)) {
    if(verbose)
      std::cout << "[qa_cs_top] Got: " << status << " Expected: " << e_status << "\n";
    shutdown_all(PMT_F);
    return;
  } else {
    if(verbose)
      std::cout << "[qa_cs_top] Received expected response for message " 
                << d_nrecvd << " (" << event << ")\n";
  }

  if(d_nrecvd == d_nmsgs_to_recv)
    shutdown_all(PMT_T);
}

REGISTER_MBLOCK_CLASS(qa_cs_top);

// ----------------------------------------------------------------------------------------------

void 
qa_inband_usrp_server::test_open_close()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  // std::cout << "\n\n----------------------------\n";
  // std::cout << "    RUNNING OPEN/CLOSE TESTS  \n";

  rt->run("top", "qa_open_close_top", PMT_F, &result);

  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

void 
qa_inband_usrp_server::test_chan_allocation()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  // std::cout << "\n\n----------------------------\n";
  // std::cout << "    RUNNING ALLOCATION TESTS  \n";

  rt->run("qa_alloc_top", "qa_alloc_top", PMT_F, &result);
  
  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

void
qa_inband_usrp_server::test_chan_deallocation()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  // std::cout << "\n\n----------------------------\n";
  // std::cout << "  RUNNING DEALLOCATION TESTS  \n";

  rt->run("qa_dealloc_top", "qa_dealloc_top", PMT_F, &result);
  
  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

void
qa_inband_usrp_server::test_tx()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  // std::cout << "\n\n-----------------\n";
  // std::cout << "  RUNNING TX TESTS  \n";

  rt->run("top", "qa_tx_top", PMT_F, &result);
  
  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

void
qa_inband_usrp_server::test_rx()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  // std::cout << "\n\n-----------------\n";
  // std::cout << "  RUNNING RX TESTS  \n";

  rt->run("top", "qa_rx_top", PMT_F, &result);
  
  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

void
qa_inband_usrp_server::test_cs()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  // std::cout << "\n\n-----------------\n";
  // std::cout << "  RUNNING CS TESTS  \n";

  rt->run("top", "qa_cs_top", PMT_F, &result);
  
  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}

void
qa_inband_usrp_server::test_rid()
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  // std::cout << "\n\n-----------------\n";
  // std::cout << "  RUNNING RID TESTS  \n";

  rt->run("top", "qa_rid_top", PMT_F, &result);
  
  CPPUNIT_ASSERT(pmt_equal(PMT_T, result));
}
