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

#include <stdio.h>
#include <iostream>
#include <usrp_inband_usb_packet.h>
#include <mb_mblock.h>
#include <mb_runtime.h>
#include <mb_protocol_class.h>
#include <mb_class_registry.h>
#include <pmt.h>
#include "usrp_standard.h"

typedef usrp_inband_usb_packet transport_pkt; 

// Signal set for the USRP server
static pmt_t s_cmd_open = pmt_intern("cmd-open");
static pmt_t s_response_open = pmt_intern("response-open");
static pmt_t s_cmd_close = pmt_intern("cmd-close");
static pmt_t s_response_close = pmt_intern("response-close");
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
static pmt_t s_cmd_xmit_raw_frame  = pmt_intern("cmd-xmit-raw-frame");
static pmt_t s_response_xmit_raw_frame = pmt_intern("response-xmit-raw-frame");

bool loopback_p = false;
bool counting_p = false;
bool fake_usrp_p = false;
char *prog_name;

static void
set_progname (char *path)
{
  char *p = strrchr (path, '/');
  if (p != 0)
    prog_name = p+1;
  else
    prog_name = path;
}

static void
usage()
{
  fprintf (stderr, "usage: %s [-l]\n", prog_name);
  fprintf (stderr, "  [-l] digital loopback in FPGA\n");
  fprintf (stderr, "  [-c] counting in FPGA\n");
  fprintf (stderr, "  [-f] fake usrp\n");

  exit(1);
}

int
main(int argc, char **argv)
{
  int ch;

  set_progname(argv[0]);

  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_T;

  while ((ch = getopt(argc, argv, "flc")) != EOF) {
    switch(ch) {
    
      case 'l':
        loopback_p = true;
        break;

      case 'c':
        counting_p = true;
        break;
      
      case 'f':
        fake_usrp_p = true;
        break;

      default:
        usage();
    }
  }


  std::cout << "[test_usrp_inband] Starting...\n";

  rt->run("top", "test_usrp_inband_top", PMT_F, &result);
}

class test_usrp_inband_top : public mb_mblock
{
  mb_port_sptr d_tx;
  mb_port_sptr d_cs;

  long d_tx_chan;

 public:
  test_usrp_inband_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~test_usrp_inband_top();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void open_usrp();
  void close_usrp();
  void check_message(mb_message_sptr msg);
  void allocate_channel();
  void send_packets();
};

test_usrp_inband_top::test_usrp_inband_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{ 
  std::cout << "[TEST_USRP_INBAND_TOP] Initializing...\n";
  
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);
  
  // Test the TX side

  // Pass a dictionary to usrp_server which specifies which interface to use, the stub or USRP
  pmt_t usrp_server_dict = pmt_make_dict();

  if(fake_usrp_p)
    pmt_dict_set(usrp_server_dict, pmt_intern("usrp-interface"), pmt_intern("usrp_usb_interface_stub"));

  define_component("server", "usrp_server", usrp_server_dict);
  connect("self", "tx0", "server", "tx0");
  connect("self", "cs", "server", "cs");
}

test_usrp_inband_top::~test_usrp_inband_top()
{
}

void
test_usrp_inband_top::initial_transition()
{
  open_usrp();
}

void
test_usrp_inband_top::handle_message(mb_message_sptr msg)
{
  pmt_t event = msg->signal();		// the "name" of the message
  pmt_t port_id = msg->port_id();	// which port it came in on
  pmt_t data = msg->data();
  pmt_t metadata = msg->metadata();
  pmt_t status;

  if (pmt_eq(port_id, d_cs->port_symbol())) {	// message came in on our control/status port

    //---------- OPEN RESPONSE ----------//
    if (pmt_eq(event, s_response_open)) {
      status = pmt_nth(1, data);

      if(pmt_eq(status, PMT_T)) {
        std::cout << "[TEST_USRP_INBAND_TOP] Success opening USRP\n";
      }
      else {
        std::cout << "[TEST_USRP_INBAND_TOP] Received error message opening USRP\n";
        shutdown_all(PMT_F);
      }

      allocate_channel();

      return;
    }
    //--------- CLOSE RESPONSE -----------//
    else if (pmt_eq(event, s_response_close)) {
      status = pmt_nth(1, data);
      
      if(pmt_eq(status, PMT_T)) {
        std::cout << "[TEST_USRP_INBAND_TOP] Successfully closed USRP\n";
      }
      else {
        std::cout << "[TEST_USRP_INBAND_TOP] Received error message closing USRP\n";
        shutdown_all(PMT_F);
      }

      shutdown_all(PMT_T);

      return;
    }
  }
    
  if (pmt_eq(port_id, d_tx->port_symbol())) {

    //---------- ALLOCATE RESPONSE ---------//
    if(pmt_eq(event, s_response_allocate_channel)) {
      status = pmt_nth(1, data);
      pmt_t channel = pmt_nth(2, data);

      if(pmt_eq(status, PMT_T)) {
        d_tx_chan = pmt_to_long(channel);
        std::cout << "[TEST_USRP_INBAND_TOP] Received allocation on channel " << d_tx_chan << "\n";
      }
      else {
        std::cout << "[TEST_USRP_INBAND_TOP] Error allocating channel\n";
        shutdown_all(PMT_F);
      }
      
      send_packets();

      return;
    }
    //----------- XMIT RESPONSE ------------//
    else if(pmt_eq(event, s_response_xmit_raw_frame)) {
      status = pmt_nth(1, data);

      if(pmt_eq(status, PMT_T)) {
        std::cout << "[TEST_USRP_INBAND_TOP] Transmission successful\n";
      }
      else {
        std::cout << "[TEST_USRP_INBAND_TOP] Failed transmission\n";
        shutdown_all(PMT_F);
      }

      close_usrp();

      return;
    }
  }

  std::cout << "[TEST_USRP_INBAND_TOP] Received unhandled message: " << event << "\n";
}

void
test_usrp_inband_top::allocate_channel()
{
  std::cout << "[TEST_USRP_INBAND_TOP] Requesting channel allocation...\n";
  
  d_tx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(1)));
}

void
test_usrp_inband_top::send_packets()
{
  std::cout << "[TEST_USRP_INBAND_TOP] Sending single packet..\n";
  d_tx->send(s_cmd_xmit_raw_frame, pmt_list4(pmt_from_long(1), pmt_from_long(d_tx_chan), pmt_make_u32vector(transport_pkt::max_payload()/4, 0), pmt_from_long(0)));
  
}

void
test_usrp_inband_top::open_usrp()
{
  pmt_t usrp = pmt_from_long(0);

  long rx_mode = 0;

  if(loopback_p)
    rx_mode |= usrp_standard_rx::FPGA_MODE_LOOPBACK;
  if(counting_p)
    rx_mode |= usrp_standard_rx::FPGA_MODE_COUNTING;

  d_cs->send(s_cmd_open, pmt_list2(PMT_NIL, usrp));
}

void
test_usrp_inband_top::close_usrp()
{
  d_cs->send(s_cmd_close, pmt_list1(PMT_NIL));
}

REGISTER_MBLOCK_CLASS(test_usrp_inband_top);
