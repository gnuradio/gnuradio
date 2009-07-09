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

#include <mblock/mblock.h>
#include <mblock/runtime.h>
#include <mblock/protocol_class.h>
#include <mblock/exception.h>
#include <mblock/msg_queue.h>
#include <mblock/message.h>
//#include <mb_mblock_impl.h>
#include <mblock/msg_accepter.h>
#include <mblock/class_registry.h>
#include <pmt.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>

// Include the symbols needed for communication with USRP server
#include <symbols_usrp_server_cs.h>
#include <symbols_usrp_channel.h>
#include <symbols_usrp_low_level_cs.h>
#include <symbols_usrp_tx.h>
#include <symbols_usrp_rx.h>

static bool verbose = true;

class test_usrp_inband_registers : public mb_mblock
{

  mb_port_sptr  d_tx;   // Ports connected to the USRP server
  mb_port_sptr  d_rx;
  mb_port_sptr  d_cs;

  pmt_t   d_tx_chan;    // Returned channel from TX allocation
  pmt_t   d_rx_chan;    // Returned channel from RX allocation

  pmt_t   d_which_usrp; // The USRP to use for the test

  long    d_warm_msgs;  // The number of messages to 'warm' the USRP
  long    d_warm_recvd; // The number of msgs received in the 'warm' state

  // Keep track of current state
  enum state_t {
    INIT,
    OPENING_USRP,
    ALLOCATING_CHANNELS,
    WRITE_REGISTER,
    READ_REGISTER,
    CLOSING_CHANNELS,
    CLOSING_USRP,
  };
  state_t d_state;

 public:
  test_usrp_inband_registers(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~test_usrp_inband_registers();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void opening_usrp();
  void allocating_channels();
  void write_register();
  void read_register();
  void closing_channels();
  void closing_usrp();
  void enter_receiving();
  void build_and_send_ping();
};


int
main (int argc, char **argv)
{
  // handle any command line args here

  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  rt->run("top", "test_usrp_inband_registers", PMT_F, &result);
}


test_usrp_inband_registers::test_usrp_inband_registers(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
  d_tx_chan(PMT_NIL),
  d_rx_chan(PMT_NIL),
  d_which_usrp(pmt_from_long(0)),
  d_state(INIT)
{
  
  // A dictionary is used to pass parameters to the USRP
  pmt_t usrp_dict = pmt_make_dict();

  // Specify the RBF to use
  pmt_dict_set(usrp_dict,
               pmt_intern("rbf"),
               pmt_intern("inband_1rxhb_1tx.rbf"));

  // Set TX and RX interpolations
  pmt_dict_set(usrp_dict,
               pmt_intern("interp-tx"),
               pmt_from_long(128));

  pmt_dict_set(usrp_dict,
               pmt_intern("decim-rx"),
               pmt_from_long(16));
  
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);

  // Create an instance of USRP server and connect ports
  define_component("server", "usrp_server", usrp_dict);
  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");

}

test_usrp_inband_registers::~test_usrp_inband_registers()
{
}

void
test_usrp_inband_registers::initial_transition()
{
  opening_usrp();
}

// Handle message reads all incoming messages from USRP server which will be
// initialization and ping responses.  We perform actions based on the current
// state and the event (ie, ping response)
void
test_usrp_inband_registers::handle_message(mb_message_sptr msg)
{
  pmt_t event = msg->signal();
  pmt_t data = msg->data();
  pmt_t port_id = msg->port_id();

  pmt_t handle = PMT_F;
  pmt_t status = PMT_F;
  std::string error_msg;

  // Dispatch based on state
  switch(d_state) {

    //----------------------------- OPENING_USRP ----------------------------//
    // We only expect a response from opening the USRP which should be succesful
    // or failed.
    case OPENING_USRP:
      
      if(pmt_eq(event, s_response_open)) {

        status = pmt_nth(1, data);          // failed/succes
        
        if(pmt_eq(status, PMT_T)) {
          allocating_channels();
          return;
        }
        else {
          error_msg = "failed to open usrp:";
          goto bail;
        }

      }

      goto unhandled;   // all other messages not handled in this state
      
    
    //----------------------- ALLOCATING CHANNELS --------------------//
    // When allocating channels, we need to wait for 2 responses from
    // USRP server: one for TX and one for RX.  Both are initialized to
    // NIL so we know to continue to the next state once both are set.
    case ALLOCATING_CHANNELS:

      // A TX allocation response
      if(pmt_eq(event, s_response_allocate_channel)
          && pmt_eq(d_tx->port_symbol(), port_id)) 
      {
        status = pmt_nth(1, data);
        
        // If successful response, extract the channel
        if(pmt_eq(status, PMT_T)) {
          
          d_tx_chan = pmt_nth(2, data);

          if(verbose)
            std::cout << "[TEST_USRP_INBAND_PING] Received TX allocation"
                      << " on channel " << d_tx_chan << std::endl;

          // If the RX has also been allocated already, we can continue
          if(!pmt_eqv(d_rx_chan, PMT_NIL)) {
            enter_receiving();
            write_register();
          }

          return;
        }
        else {  // TX allocation failed
          error_msg = "failed to allocate TX channel:";
          goto bail;
        }
      }
      
      // A RX allocation response
      if(pmt_eq(event, s_response_allocate_channel)
          && pmt_eq(d_rx->port_symbol(), port_id)) 
      {
        status = pmt_nth(1, data);
        
        // If successful response, extract the channel
        if(pmt_eq(status, PMT_T)) {
          
          d_rx_chan = pmt_nth(2, data);

          if(verbose)
            std::cout << "[TEST_USRP_INBAND_PING] Received RX allocation"
                      << " on channel " << d_rx_chan << std::endl;

          // If the TX has also been allocated already, we can continue
          if(!pmt_eqv(d_tx_chan, PMT_NIL)) {
            enter_receiving();
            write_register();
          }

          return;
        }
        else {  // RX allocation failed
          error_msg = "failed to allocate RX channel:";
          goto bail;
        }
      }

      goto unhandled;

    //-------------------------- WRITE REGISTER ----------------------------//
    // In the write register state, we do not expect to receive any messages
    // since the write does not directly generate a response until the USRP
    // responds.
    case WRITE_REGISTER:
      goto unhandled;

    //-------------------------- READ REGISTER ----------------------------//
    // In the read register state, we only expect a read register response back
    // that has the value we expect to have in it.  We read the response, ensure
    // that the read was successful and display the register value.
    case READ_REGISTER:

      if(pmt_eq(event, s_response_from_control_channel)
          && pmt_eq(d_tx->port_symbol(), port_id))
      {
        status = pmt_nth(1, data);

        // If the read was successful, we extract the subpacket information
        if(pmt_eq(status, PMT_T)) {
          
          pmt_t subp = pmt_nth(2, data);      // subpacket should be the read reg reply

          pmt_t subp_sig  = pmt_nth(0, subp);
          pmt_t subp_data = pmt_nth(1, subp);

          if(!pmt_eqv(subp_sig, s_op_read_reg_reply)) {
            error_msg = "received improper subpacket when expecting reg reply.";
            goto bail;
          }

          pmt_t rid     = pmt_nth(0, subp_data);
          pmt_t reg_num = pmt_nth(1, subp_data);
          pmt_t reg_val = pmt_nth(2, subp_data);

          if(verbose)
            std::cout << "[TEST_USRP_INBAND_REGISTERS] Received read reg reply "
                      << "("
                      << "RID: " << rid << ", " 
                      << "Reg: " << reg_num << ", "
                      << "Val: " << reg_val
                      << ")\n";
          
          // read_register();  FIX ME STATE TRANSITION
          return;

        } else {  // bail on unsuccessful write
          error_msg = "failed to write to register.";
          goto bail;
        }
      }
      goto unhandled;

    case CLOSING_CHANNELS:
      goto unhandled;

    case CLOSING_USRP:
      goto unhandled;

    case INIT:
      goto unhandled;

  }
 
 // An error occured, print it, and shutdown all m-blocks
 bail:
  std::cerr << error_msg << data
      	    << "status = " << status << std::endl;
  shutdown_all(PMT_F);
  return;

 // Received an unhandled message for a specific state
 unhandled:
  if(verbose && !pmt_eq(event, s_response_recv_raw_samples))
    std::cout << "test_usrp_inband_tx: unhandled msg: " << msg
              << "in state "<< d_state << std::endl;

}


// Sends a command to USRP server to open up a connection to the
// specified USRP, which is defaulted to USRP 0 on the system
void
test_usrp_inband_registers::opening_usrp()
{

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_PING] Opening USRP " 
              << d_which_usrp << std::endl;

  d_cs->send(s_cmd_open, pmt_list2(PMT_NIL, d_which_usrp));
  d_state = OPENING_USRP;
}

// RX and TX channels must be allocated so that the USRP server can
// properly share bandwidth across multiple USRPs.  No commands will be
// successful to the USRP through the USRP server on the TX or RX channels until
// a bandwidth allocation has been received.
void
test_usrp_inband_registers::allocating_channels()
{
  d_state = ALLOCATING_CHANNELS;

  long capacity = (long) 16e6;
  d_tx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
  d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
}

// After allocating the channels, a write register command will be sent to the
// USRP.
void
test_usrp_inband_registers::write_register()
{
  d_state = WRITE_REGISTER;

  long reg = 0;

  d_tx->send(s_cmd_to_control_channel,    // C/S packet
             pmt_list2(PMT_NIL,           // invoc handle
                       pmt_list1(
                            pmt_list2(s_op_write_reg, 
                                      pmt_list2(
                                      pmt_from_long(reg), 
                                      pmt_from_long(0xbeef))))));

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_REGISTERS] Writing 0xbeef to " 
              << reg << std::endl;

  read_register();  // immediately transition to read the register
}

// Temporary: for testing pings
void
test_usrp_inband_registers::build_and_send_ping()
{
  
  d_tx->send(s_cmd_to_control_channel,
             pmt_list2(PMT_NIL, pmt_list1(pmt_list2(s_op_ping_fixed,
                                                    pmt_list2(pmt_from_long(0),
                                                              pmt_from_long(0))))));

  std::cout << "[TEST_USRP_INBAND_CS] Ping sent" << std::endl;
}

// After writing to the register, we want to read the value back and ensure that
// it is the same value that we wrote.
void
test_usrp_inband_registers::read_register()
{
  d_state = READ_REGISTER;

  long reg = 9;

  d_tx->send(s_cmd_to_control_channel,    // C/S packet
             pmt_list2(PMT_NIL,           // invoc handle
                       pmt_list1(
                            pmt_list2(s_op_read_reg, 
                                      pmt_list2(
                                      pmt_from_long(0),   // rid 
                                      pmt_from_long(reg))))));
  if(verbose)
    std::cout << "[TEST_USRP_INBAND_REGISTERS] Reading from register " 
              << reg << std::endl;
}

// Used to enter the receiving state
void
test_usrp_inband_registers::enter_receiving()
{
  d_rx->send(s_cmd_start_recv_raw_samples,
             pmt_list2(PMT_F,
                       d_rx_chan));
}

REGISTER_MBLOCK_CLASS(test_usrp_inband_registers);
