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
#include <mblock/runtime.h>
#include <mb_runtime_nop.h>		// QA only
#include <mblock/protocol_class.h>
#include <mblock/exception.h>
#include <mblock/msg_queue.h>
#include <mblock/message.h>
#include <mb_mblock_impl.h>
#include <mblock/msg_accepter.h>
#include <mblock/class_registry.h>
#include <pmt.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

// Include the symbols needed for communication with USRP server
#include <symbols_usrp_server_cs.h>
#include <symbols_usrp_channel.h>
#include <symbols_usrp_low_level_cs.h>
#include <symbols_usrp_rx.h>

static bool verbose = true;

class test_usrp_rx : public mb_mblock
{
  mb_port_sptr 	d_rx;
  mb_port_sptr 	d_cs;
  pmt_t		d_rx_chan0, d_rx_chan1;

  enum state_t {
    INIT,
    OPENING_USRP,
    ALLOCATING_CHANNEL,
    RECEIVING,
    CLOSING_CHANNEL,
    CLOSING_USRP,
  };

  state_t	d_state;

  std::ofstream d_ofile;

  long d_samples_recvd;
  long d_samples_to_recv;

 public:
  test_usrp_rx(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~test_usrp_rx();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void open_usrp();
  void close_usrp();
  void allocate_channel();
  void send_packets();
  void enter_receiving();
  void build_and_send_next_frame();
  void handle_response_recv_raw_samples(pmt_t invocation_handle);
  void enter_closing_channel();
};

test_usrp_rx::test_usrp_rx(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
    d_rx_chan0(PMT_NIL), d_rx_chan1(PMT_NIL),
    d_samples_recvd(0),
    d_samples_to_recv(20e6)
{ 
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);
  
  // Pass a dictionary to usrp_server which specifies which interface to use, the stub or USRP
  pmt_t usrp_dict = pmt_make_dict();
  
  // To test the application without a USRP
  bool fake_usrp_p = false;
  if(fake_usrp_p) {
    pmt_dict_set(usrp_dict, 
                 pmt_intern("fake-usrp"),
		             PMT_T);
  }

  // Specify the RBF to use
  pmt_dict_set(usrp_dict,
               pmt_intern("rbf"),
               pmt_intern("inband_2rxhb_2tx.rbf"));

  pmt_dict_set(usrp_dict,
               pmt_intern("decim-rx"),
               pmt_from_long(64));

  define_component("server", "usrp_server", usrp_dict);

  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");

}

test_usrp_rx::~test_usrp_rx()
{
}

void
test_usrp_rx::initial_transition()
{
  open_usrp();
}

void
test_usrp_rx::handle_message(mb_message_sptr msg)
{
  pmt_t	event = msg->signal();
  pmt_t data = msg->data();

  pmt_t handle = PMT_F;
  pmt_t status = PMT_F;
  std::string error_msg;
  
  switch(d_state){
    
    //----------------------------- OPENING_USRP ----------------------------//
    // We only expect a response from opening the USRP which should be succesful
    // or failed.
    case OPENING_USRP:
      if (pmt_eq(event, s_response_open)){
        status = pmt_nth(1, data);
        if (pmt_eq(status, PMT_T)){
          allocate_channel();
          return;
        }
        else {
          error_msg = "failed to open usrp:";
          goto bail;
        }
      }
      goto unhandled;
      
    //----------------------- ALLOCATING CHANNELS --------------------//
    // Allocate an RX channel to perform the overrun test.
    case ALLOCATING_CHANNEL:
      if (pmt_eq(event, s_response_allocate_channel)){
        status = pmt_nth(1, data);
        if(pmt_eqv(d_rx_chan0, PMT_NIL))
          d_rx_chan0 = pmt_nth(2, data);
        else
          d_rx_chan1 = pmt_nth(2, data);

        if (pmt_eq(status, PMT_T) && !pmt_eqv(d_rx_chan1, PMT_NIL)){
          enter_receiving();
          return;
        }
        else if(pmt_eq(status, PMT_F)){
          error_msg = "failed to allocate channel:";
          goto bail;
        }
        return;
      }
      goto unhandled;

    //--------------------------- RECEIVING ------------------------------//
    // In the receiving state, we receive samples until the specified amount
    // while counting the number of overruns.
    case RECEIVING:
      if (pmt_eq(event, s_response_recv_raw_samples)){
        status = pmt_nth(1, data);

        if (pmt_eq(status, PMT_T)){
          handle_response_recv_raw_samples(data);
          return;
        }
        else {
          error_msg = "bad response-xmit-raw-frame:";
          goto bail;
        }
      }
      goto unhandled;
    
    //------------------------- CLOSING CHANNEL ----------------------------//
    // Check deallocation response for the RX channel 
    case CLOSING_CHANNEL:
      if (pmt_eq(event, s_response_deallocate_channel)){
        status = pmt_nth(1, data);

        if (pmt_eq(status, PMT_T)){
          close_usrp();
          return;
        }
        else {
          error_msg = "failed to deallocate channel:";
          goto bail;
        }
      }

      // Alternately, we ignore all response recv samples while waiting for the
      // channel to actually close
      if (pmt_eq(event, s_response_recv_raw_samples))
        return;

      goto unhandled;

    //--------------------------- CLOSING USRP ------------------------------//
    // Once we have received a successful USRP close response, we shutdown all
    // mblocks and exit.
    case CLOSING_USRP:
      if (pmt_eq(event, s_response_close)){
        status = pmt_nth(1, data);

        if (pmt_eq(status, PMT_T)){
          fflush(stdout);
          shutdown_all(PMT_T);
          return;
        }
        else {
          error_msg = "failed to close USRP:";
          goto bail;
        }
      }
      goto unhandled;

    default:
      goto unhandled;
  }
  return;

 // An error occured, print it, and shutdown all m-blocks
 bail:
  std::cerr << error_msg << data
	    << "status = " << status << std::endl;
  shutdown_all(PMT_F);
  return;

 // Received an unhandled message for a specific state
 unhandled:
  if(verbose && !pmt_eq(event, pmt_intern("%shutdown")))
    std::cout << "test_usrp_inband_tx: unhandled msg: " << msg
              << "in state "<< d_state << std::endl;
}


void
test_usrp_rx::open_usrp()
{
  pmt_t which_usrp = pmt_from_long(0);

  d_cs->send(s_cmd_open, pmt_list2(PMT_NIL, which_usrp));
  d_state = OPENING_USRP;
  
  if(verbose)
    std::cout << "[TEST_USRP_INBAND_RX] Opening the USRP\n";
}

void
test_usrp_rx::close_usrp()
{

  d_cs->send(s_cmd_close, pmt_list1(PMT_NIL));
  d_state = CLOSING_USRP;
  
  if(verbose)
    std::cout << "[TEST_USRP_INBAND_RX] Closing the USRP\n";
}

void
test_usrp_rx::allocate_channel()
{
  long capacity = (long) 16e6;
  d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
  d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
  d_state = ALLOCATING_CHANNEL;
  
  if(verbose)
    std::cout << "[TEST_USRP_INBAND_RX] Requesting RX channel allocation\n";
}

void
test_usrp_rx::enter_receiving()
{
  d_state = RECEIVING;

  d_rx->send(s_cmd_start_recv_raw_samples,
             pmt_list2(PMT_F,
                       d_rx_chan0));
  
  d_rx->send(s_cmd_start_recv_raw_samples,
             pmt_list2(PMT_F,
                       d_rx_chan1));

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_RX] Receiving...\n";
}

void
test_usrp_rx::handle_response_recv_raw_samples(pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);
  pmt_t v_samples = pmt_nth(2, data);
  pmt_t timestamp = pmt_nth(3, data);
  pmt_t channel = pmt_nth(4, data);
  pmt_t properties = pmt_nth(5, data);

  d_samples_recvd += pmt_length(v_samples) / 4;

  // Check for overrun
  if(!pmt_is_dict(properties)) {
    std::cout << "[TEST_USRP_INBAND_RX] Recv samples dictionary is improper\n";
    return;
  }

  // Check if the number samples we have received meets the test
  if(d_samples_recvd >= d_samples_to_recv) {
    d_rx->send(s_cmd_stop_recv_raw_samples, pmt_list2(PMT_NIL, d_rx_chan0));
    d_rx->send(s_cmd_stop_recv_raw_samples, pmt_list2(PMT_NIL, d_rx_chan1));
    enter_closing_channel();
    return;
  }

}

void
test_usrp_rx::enter_closing_channel()
{
  d_state = CLOSING_CHANNEL;

  d_rx->send(s_cmd_deallocate_channel, pmt_list2(PMT_NIL, d_rx_chan0));
  d_rx->send(s_cmd_deallocate_channel, pmt_list2(PMT_NIL, d_rx_chan1));
  
  if(verbose)
    std::cout << "[TEST_USRP_INBAND_RX] Deallocating RX channel\n";
}

REGISTER_MBLOCK_CLASS(test_usrp_rx);


// ----------------------------------------------------------------

int
main (int argc, char **argv)
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  rt->run("top", "test_usrp_rx", PMT_F, &result);

}
