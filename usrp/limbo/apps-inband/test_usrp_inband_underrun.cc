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
#include <mblock/protocol_class.h>
#include <mblock/exception.h>
#include <mblock/msg_queue.h>
#include <mblock/message.h>
#include <mblock/msg_accepter.h>
#include <mblock/class_registry.h>
#include <pmt.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <ui_nco.h>

// Include the symbols needed for communication with USRP server
#include <symbols_usrp_server_cs.h>
#include <symbols_usrp_channel.h>
#include <symbols_usrp_low_level_cs.h>
#include <symbols_usrp_tx.h>
#include <symbols_usrp_rx.h>

static bool verbose = true;

class test_usrp_inband_underrun : public mb_mblock
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
    TRANSMITTING,
    CLOSING_CHANNELS,
    CLOSING_USRP,
  };
  state_t d_state;
  
  long		d_nsamples_to_send;
  long		d_nsamples_xmitted;
  long		d_nframes_xmitted;
  long		d_samples_per_frame;
  bool		d_done_sending;

  // for generating sine wave output
  ui_nco<float,float>	d_nco;
  double	        d_amplitude;

  long d_n_underruns;

 public:
  test_usrp_inband_underrun(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~test_usrp_inband_underrun();
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
  void enter_transmitting();
  void build_and_send_ping();
  void build_and_send_next_frame();
  void handle_xmit_response(pmt_t handle);
  void handle_recv_response(pmt_t dict);
};


int
main (int argc, char **argv)
{
  // handle any command line args here

  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  rt->run("top", "test_usrp_inband_underrun", PMT_F, &result);
}


test_usrp_inband_underrun::test_usrp_inband_underrun(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
  d_tx_chan(PMT_NIL),
  d_rx_chan(PMT_NIL),
  d_which_usrp(pmt_from_long(0)),
  d_state(INIT),
  d_nsamples_to_send((long) 27e6),
  d_nsamples_xmitted(0),
  d_nframes_xmitted(0),
  d_samples_per_frame(d_nsamples_to_send),	// full packet

  d_done_sending(false),
  d_amplitude(16384),
  d_n_underruns(0)
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
               pmt_from_long(64));

  pmt_dict_set(usrp_dict,
               pmt_intern("decim-rx"),
               pmt_from_long(128));
  
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);

  // Create an instance of USRP server and connect ports
  define_component("server", "usrp_server", usrp_dict);
  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");

  // initialize NCO
  double freq = 100e3;
  int interp = 32;			    // 32 -> 4MS/s
  double sample_rate = 128e6 / interp;	
  d_nco.set_freq(2*M_PI * freq/sample_rate);
}

test_usrp_inband_underrun::~test_usrp_inband_underrun()
{
}

void
test_usrp_inband_underrun::initial_transition()
{
  opening_usrp();
}

// Handle message reads all incoming messages from USRP server which will be
// initialization and ping responses.  We perform actions based on the current
// state and the event (ie, ping response)
void
test_usrp_inband_underrun::handle_message(mb_message_sptr msg)
{
  pmt_t event = msg->signal();
  pmt_t data = msg->data();
  pmt_t port_id = msg->port_id();

  pmt_t handle = PMT_F;
  pmt_t status = PMT_F;
  pmt_t dict = PMT_NIL;
  std::string error_msg;
      
  // Check the recv sample responses for underruns and count
  if(pmt_eq(event, s_response_recv_raw_samples)) {
    handle = pmt_nth(0, data);
    status = pmt_nth(1, data);
    dict   = pmt_nth(4, data);

    if(pmt_eq(status, PMT_T)) {
      handle_recv_response(dict);
      return;
    }
    else {
      error_msg = "error while receiving samples:";
      goto bail;
    }
  }


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
            std::cout << "[TEST_USRP_INBAND_UNDERRUN] Received TX allocation"
                      << " on channel " << d_tx_chan << std::endl;

          // If the RX has also been allocated already, we can continue
          if(!pmt_eqv(d_rx_chan, PMT_NIL)) {
            enter_receiving();
            enter_transmitting();
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
            std::cout << "[TEST_USRP_INBAND_UNDERRUN] Received RX allocation"
                      << " on channel " << d_rx_chan << std::endl;

          // If the TX has also been allocated already, we can continue
          if(!pmt_eqv(d_tx_chan, PMT_NIL)) {
            enter_receiving();
            enter_transmitting();
          }

          return;
        }
        else {  // RX allocation failed
          error_msg = "failed to allocate RX channel:";
          goto bail;
        }
      }

      goto unhandled;

    case WRITE_REGISTER:
      goto unhandled;

    case READ_REGISTER:
      goto unhandled;

    //-------------------------- TRANSMITTING ----------------------------//
    // In the transmit state we count the number of underruns received and
    // ballpark the number with an expected count (something >1 for starters)
    case TRANSMITTING:
      
      // Check that the transmits are OK
      if (pmt_eq(event, s_response_xmit_raw_frame)){
        handle = pmt_nth(0, data);
        status = pmt_nth(1, data);

        if (pmt_eq(status, PMT_T)){
          handle_xmit_response(handle);
          return;
        }
        else {
          error_msg = "bad response-xmit-raw-frame:";
          goto bail;
        }
      }

      goto unhandled;

    //------------------------- CLOSING CHANNELS ----------------------------//
    // Check deallocation responses, once the TX and RX channels are both
    // deallocated then we close the USRP.
    case CLOSING_CHANNELS:
      
      if (pmt_eq(event, s_response_deallocate_channel)
          && pmt_eq(d_tx->port_symbol(), port_id))
      {
        status = pmt_nth(1, data);

        // If successful, set the port to NIL
        if(pmt_eq(status, PMT_T)) {
          d_tx_chan = PMT_NIL;

          if(verbose)
            std::cout << "[TEST_USRP_INBAND_UNDERRUN] Received TX deallocation\n";

          // If the RX is also deallocated, we can close the USRP
          if(pmt_eq(d_rx_chan, PMT_NIL)) 
            closing_usrp();

          return;

        } else {

          error_msg = "failed to deallocate TX channel:";
          goto bail;

        }
      }

      if (pmt_eq(event, s_response_deallocate_channel)
          && pmt_eq(d_rx->port_symbol(), port_id))
      {
        status = pmt_nth(1, data);

        // If successful, set the port to NIL
        if(pmt_eq(status, PMT_T)) {
          d_rx_chan = PMT_NIL;

          if(verbose)
            std::cout << "[TEST_USRP_INBAND_UNDERRUN] Received RX deallocation\n";

          // If the TX is also deallocated, we can close the USRP
          if(pmt_eq(d_tx_chan, PMT_NIL)) 
            closing_usrp();

          return;

        } else {
          
          error_msg = "failed to deallocate RX channel:";
          goto bail;

        }
      }

      goto unhandled;

    //--------------------------- CLOSING USRP ------------------------------//
    // Once we have received a successful USRP close response, we shutdown all
    // mblocks and exit.
    case CLOSING_USRP:
      
      if (pmt_eq(event, s_response_close)) {
        
        status = pmt_nth(1, data);

        if(pmt_eq(status, PMT_T)) {

          if(verbose)
            std::cout << "[TEST_USRP_INBAND_UNDERRUN] Successfully closed USRP\n";

          std::cout << "\nUnderruns: " << d_n_underruns << std::endl;
          fflush(stdout);

          shutdown_all(PMT_T);
          return;

        } else {

          error_msg = "failed to close USRP:";
          goto bail;
        }
      }

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
  if(verbose && !pmt_eq(event, pmt_intern("%shutdown")))
    std::cout << "test_usrp_inband_tx: unhandled msg: " << msg
              << "in state "<< d_state << std::endl;

}


// Sends a command to USRP server to open up a connection to the
// specified USRP, which is defaulted to USRP 0 on the system
void
test_usrp_inband_underrun::opening_usrp()
{

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_UNDERRUN] Opening USRP " 
              << d_which_usrp << std::endl;

  d_cs->send(s_cmd_open, pmt_list2(PMT_NIL, d_which_usrp));
  d_state = OPENING_USRP;
}

// RX and TX channels must be allocated so that the USRP server can
// properly share bandwidth across multiple USRPs.  No commands will be
// successful to the USRP through the USRP server on the TX or RX channels until
// a bandwidth allocation has been received.
void
test_usrp_inband_underrun::allocating_channels()
{
  d_state = ALLOCATING_CHANNELS;

  long capacity = (long) 16e6;
  d_tx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
  d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
}

// After allocating the channels, a write register command will be sent to the
// USRP.
void
test_usrp_inband_underrun::write_register()
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
test_usrp_inband_underrun::build_and_send_ping()
{
  
  d_tx->send(s_cmd_to_control_channel,
             pmt_list2(PMT_NIL, pmt_list1(pmt_list2(s_op_ping_fixed,
                                                    pmt_list2(pmt_from_long(0),
                                                              pmt_from_long(0))))));

  std::cout << "[TEST_USRP_INBAND_UNDERRUN] Ping sent" << std::endl;
}

// After writing to the register, we want to read the value back and ensure that
// it is the same value that we wrote.
void
test_usrp_inband_underrun::read_register()
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
    std::cout << "[TEST_USRP_INBAND_UNDERRUN] Reading from register " 
              << reg << std::endl;
}

// Used to enter the receiving state
void
test_usrp_inband_underrun::enter_receiving()
{
  d_rx->send(s_cmd_start_recv_raw_samples,
             pmt_list2(PMT_F,
                       d_rx_chan));

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_UNDERRUN] Started RX sample stream\n";
}

void
test_usrp_inband_underrun::enter_transmitting()
{
  d_state = TRANSMITTING;
  d_nsamples_xmitted = 0;

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_UNDERRUN] Entering transmit state...\n";
  
  build_and_send_next_frame();	// fire off 4 to start pipeline
  build_and_send_next_frame();
  build_and_send_next_frame();
  build_and_send_next_frame();
}

void
test_usrp_inband_underrun::build_and_send_next_frame()
{

  long nsamples_this_frame =
    std::min(d_nsamples_to_send - d_nsamples_xmitted,
	     d_samples_per_frame);

  if (nsamples_this_frame == 0){
    d_done_sending = true;
    return;
  }
    
  size_t nshorts = 2 * nsamples_this_frame;	// 16-bit I & Q
  pmt_t uvec = pmt_make_s16vector(nshorts, 0);
  size_t ignore;
  int16_t *samples = pmt_s16vector_writable_elements(uvec, ignore);

  // fill in the complex sinusoid

  for (int i = 0; i < nsamples_this_frame; i++){

    if (1){
      gr_complex s;
      d_nco.sincos(&s, 1, d_amplitude);
      // write 16-bit i & q
      samples[2*i] =   (int16_t) s.real();
      samples[2*i+1] = (int16_t) s.imag();
    }
    else {
      gr_complex s(d_amplitude, d_amplitude);

      // write 16-bit i & q
      samples[2*i] =   (int16_t) s.real();
      samples[2*i+1] = (int16_t) s.imag();
    }
  }

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_TX] Transmitting frame...\n";

  pmt_t timestamp = pmt_from_long(0xffffffff);	// NOW
  d_tx->send(s_cmd_xmit_raw_frame,
	     pmt_list4(pmt_from_long(d_nframes_xmitted),  // invocation-handle
		       d_tx_chan,			  // channel
		       uvec,				  // the samples
		       timestamp));

  d_nsamples_xmitted += nsamples_this_frame;
  d_nframes_xmitted++;

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_TX] Transmitted frame\n";

}

void
test_usrp_inband_underrun::handle_xmit_response(pmt_t handle)
{
  if (d_done_sending &&
    pmt_to_long(handle) == (d_nframes_xmitted - 1)){
    // We're done sending and have received all responses
    closing_channels();
    return;
  }

  build_and_send_next_frame();
}

void
test_usrp_inband_underrun::handle_recv_response(pmt_t dict)
{
  if(!pmt_is_dict(dict)) {
    std::cout << "[TEST_USRP_INBAND_UNDERRUN] Recv samples dictionary is improper\n";
    return;
  }

  // Read the TX interpolations
  if(pmt_t underrun = pmt_dict_ref(dict, 
                                  pmt_intern("underrun"), 
                                  PMT_NIL)) {
    if(pmt_eqv(underrun, PMT_T)) {
      d_n_underruns++;

      if(verbose && 0)
        std::cout << "[TEST_USRP_INBAND_UNDERRUN] Underrun\n";
    }
    else {
    if(verbose && 0)
      std::cout << "[TEST_USRP_INBAND_UNDERRUN] No underrun\n" << underrun <<std::endl;
    }
  } else {

    if(verbose && 0)
      std::cout << "[TEST_USRP_INBAND_UNDERRUN] No underrun\n";
  }
  
}

void
test_usrp_inband_underrun::closing_channels()
{
  d_state = CLOSING_CHANNELS;

  d_tx->send(s_cmd_deallocate_channel, pmt_list2(PMT_NIL, d_tx_chan));
  d_rx->send(s_cmd_deallocate_channel, pmt_list2(PMT_NIL, d_rx_chan));
}

void
test_usrp_inband_underrun::closing_usrp()
{
  d_state = CLOSING_USRP;

  d_cs->send(s_cmd_close, pmt_list1(PMT_NIL));
}

REGISTER_MBLOCK_CLASS(test_usrp_inband_underrun);
