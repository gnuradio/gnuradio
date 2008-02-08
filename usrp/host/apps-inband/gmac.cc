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

#include <gmac.h>

#include <mb_mblock.h>
#include <mb_runtime.h>
#include <mb_protocol_class.h>
#include <mb_exception.h>
#include <mb_msg_queue.h>
#include <mb_message.h>
#include <mb_msg_accepter.h>
#include <mb_class_registry.h>
#include <pmt.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <ui_nco.h>

#include <symbols_usrp_server_cs.h>
#include <symbols_usrp_channel.h>
#include <symbols_usrp_low_level_cs.h>
#include <symbols_usrp_tx.h>
#include <symbols_usrp_rx.h>

#include <gmac_symbols.h>

static bool verbose = true;

gmac::gmac(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(rt, instance_name, user_arg),
    d_us_rx_chan(PMT_NIL), d_us_tx_chan(PMT_NIL)
{

  // When the MAC layer is initialized, we must connect to the USRP and setup
  // channels.  We begin by defining ports to connect to the 'usrp_server' block
  // and then initialize the USRP by opening it through the 'usrp_server.'

  // Initialize the ports
  define_ports();

  // Initialize the connection to the USRP
  initialize_usrp();

}

gmac::~gmac()
{
}

// The full functionality of GMAC is based on messages passed back and forth
// between the application and a physical layer and/or usrp_server.  Each
// message triggers additional events, states, and messages to be sent.
void gmac::handle_message(mb_message_sptr msg)
{

  // The MAC functionality is dispatched based on the event, which is the
  // driving force of the MAC.  The event can be anything from incoming samples
  // to a message to change the carrier sense threshold.
  pmt_t event = msg->signal();
  pmt_t data = msg->data();
  pmt_t port_id = msg->port_id();

  pmt_t handle = PMT_F;
  pmt_t status = PMT_F;
  pmt_t dict = PMT_NIL;
  std::string error_msg;

  switch(d_state) {
    
    //---------------------------- INIT ------------------------------------//
    // In the INIT state, there should be no messages across the ports. 
    case INIT:
      error_msg = "no messages should be passed during the INIT state:"; 
      goto unhandled;

    //-------------------------- OPENING USRP -------------------------------//
    // In this state we expect a response from usrp_server over the CS channel
    // as to whether or not the opening of the USRP was successful.  If so, we
    // switch states to allocating the channels for use.
    case OPENING_USRP:

      if(pmt_eq(event, s_response_open)
          && pmt_eq(d_us_cs->port_symbol(), port_id)) {

        status = pmt_nth(1, data);          // PMT_T or PMT_F

        if(pmt_eq(status, PMT_T)) {         // on success, allocate channels!
          allocate_channels();
          return;
        }
        else {
          error_msg = "failed to open usrp:";
          goto bail;
        }

      }

      goto unhandled;   // all other messages not handled in this state

    //------------------------ ALLOCATING CHANNELS --------------------------//
    // When allocating channels, we need to wait for 2 responses from USRP
    // server: one for TX and one for RX.  Both are initialized to NIL so we
    // know to continue to the next state once both are set.
    case ALLOCATING_CHANNELS:

      // ************* TX ALLOCATION RESPONSE ***************** //
      if(pmt_eq(event, s_response_allocate_channel)
          && pmt_eq(d_us_tx->port_symbol(), port_id)) 
      {
        status = pmt_nth(1, data);
        
        if(pmt_eq(status, PMT_T)) {   // extract channel on success
          d_us_tx_chan = pmt_nth(2, data);

          if(verbose)
            std::cout << "[GMAC] Received TX allocation"
                      << " on channel " << d_us_tx_chan << std::endl;

          // If the RX has also been allocated already, we can continue
          if(!pmt_eqv(d_us_rx_chan, PMT_NIL)) {
            //enter_receiving();
            initialize_gmac();
          }

          return;
        }
        else {  // TX allocation failed
          error_msg = "failed to allocate TX channel:";
          goto bail;
        }
      }
      
      // ************* RX ALLOCATION RESPONSE ****************//
      if(pmt_eq(event, s_response_allocate_channel)
          && pmt_eq(d_us_rx->port_symbol(), port_id)) 
      {
        status = pmt_nth(1, data);
        
        if(pmt_eq(status, PMT_T)) {
          
          d_us_rx_chan = pmt_nth(2, data);

          if(verbose)
            std::cout << "[GMAC] Received RX allocation"
                      << " on channel " << d_us_rx_chan << std::endl;

          // If the TX has also been allocated already, we can continue
          if(!pmt_eqv(d_us_tx_chan, PMT_NIL)) {
            //enter_receiving();
            initialize_gmac();
          }

          return;
        }
        else {  // RX allocation failed
          error_msg = "failed to allocate RX channel:";
          goto bail;
        }
      }

      goto unhandled;
    
    //----------------------------- INIT GMAC --------------------------------//
    // In the INIT_GMAC state, now that the USRP is initialized we can do things
    // like right the carrier sense threshold to the FPGA register.
    case INIT_GMAC:
      goto unhandled;

    
    //----------------------------- IDLE ------------------------------------//
    // In the idle state the MAC is not quite 'idle', it is just not doing
    // anything specific.  It is still being passive with data between the
    // application and the lower layer.
    case IDLE:
      
      //-------- TX PORT ----------------------------------------------------//
      if(pmt_eq(d_tx->port_symbol(), port_id)) {

        //-------- INCOMING PACKET ------------------------------------------//
        if(pmt_eq(event, s_cmd_tx_pkt)) {
          handle_cmd_tx_pkt(data);
          return;
        }

      }

      //--------- USRP TX PORT ----------------------------------------------//
      if(pmt_eq(d_us_tx->port_symbol(), port_id)) {

        //-------- INCOMING PACKET RESPONSE ---------------------------------//
        if(pmt_eq(event, s_response_xmit_raw_frame)) {
          handle_response_xmit_raw_frame(data);
          return;
        }

      }

      //--------- CS PORT ---------------------------------------------------//
      if(pmt_eq(d_cs->port_symbol(), port_id)) {
        
        //------- ENABLE CARRIER SENSE --------------------------------------//
        if(pmt_eq(event, s_cmd_carrier_sense_enable)) {
          handle_cmd_carrier_sense_enable(data);
          return;
        }
        
        //------- CARRIER SENSE THRESHOLD -----------------------------------//
        if(pmt_eq(event, s_cmd_carrier_sense_threshold)) {
          handle_cmd_carrier_sense_threshold(data);
          return;
        }

        //------- CARRIER SENSE DEADLINE ------------------------------------//
        if(pmt_eq(event, s_cmd_carrier_sense_deadline)) {
          handle_cmd_carrier_sense_deadline(data);
          return;
        }

        //------- DISABLE CARRIER SENSE -------------------------------------//
        if(pmt_eq(event, s_cmd_carrier_sense_disable)) {
          handle_cmd_carrier_sense_disable(data);
          return;
        }

      }

      goto unhandled;

    //------------------------ CLOSING CHANNELS -----------------------------//
    case CLOSING_CHANNELS:

      if (pmt_eq(event, s_response_deallocate_channel)
          && pmt_eq(d_us_tx->port_symbol(), port_id))
      {
        status = pmt_nth(1, data);

        if(pmt_eq(status, PMT_T)) {
          d_us_tx_chan = PMT_NIL;

          if(verbose)
            std::cout << "[GMAC] Received TX deallocation\n";

          // If the RX is also deallocated, we can close the USRP
          if(pmt_eq(d_us_rx_chan, PMT_NIL)) 
            close_usrp();

          return;

        } else {

          error_msg = "failed to deallocate TX channel:";
          goto bail;

        }
      }

      if (pmt_eq(event, s_response_deallocate_channel)
          && pmt_eq(d_us_rx->port_symbol(), port_id))
      {
        status = pmt_nth(1, data);

        // If successful, set the port to NIL
        if(pmt_eq(status, PMT_T)) {
          d_us_rx_chan = PMT_NIL;

          if(verbose)
            std::cout << "[GMAC] Received RX deallocation\n";

          // If the TX is also deallocated, we can close the USRP
          if(pmt_eq(d_us_tx_chan, PMT_NIL)) 
            close_usrp();

          return;

        } else {
          
          error_msg = "failed to deallocate RX channel:";
          goto bail;

        }
      }

      goto unhandled;

    //-------------------------- CLOSING USRP -------------------------------//
    case CLOSING_USRP:
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
  if(0 && verbose && !pmt_eq(event, pmt_intern("%shutdown")))
    std::cout << "[GMAC] unhandled msg: " << msg
              << "in state "<< d_state << std::endl;
}

// The MAC layer connects to 'usrp_server' which has a control/status channel,
// a TX, and an RX port.  The MAC layer can then relay TX/RX data back and
// forth to the application, or a physical layer once available.
void gmac::define_ports()
{
  // Ports we use to connect to usrp_server
  d_us_tx = define_port("us-tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_us_rx = define_port("us-rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_us_cs = define_port("us-cs", "usrp-server-cs", false, mb_port::INTERNAL);
  
  // Ports applications used to connect to us
  d_tx = define_port("tx0", "gmac-tx", true, mb_port::EXTERNAL);
  d_rx = define_port("rx0", "gmac-rx", true, mb_port::EXTERNAL);
  d_cs = define_port("cs", "gmac-cs", true, mb_port::EXTERNAL);
}

// To initialize the USRP we must pass several parameters to 'usrp_server' such
// as the RBF to use, and the interpolation/decimation rate.  The MAC layer will
// then pass these parameters to the block with a message to establish the
// connection to the USRP.
void gmac::initialize_usrp()
{

  if(verbose)
    std::cout << "[GMAC] Initializing USRP\n";

  // The initialization parameters are passed to usrp_server via a PMT
  // dictionary.
  pmt_t usrp_dict = pmt_make_dict();

  // Specify the RBF to use
  pmt_dict_set(usrp_dict,
               pmt_intern("rbf"),
               pmt_intern("test2.rbf"));

  pmt_dict_set(usrp_dict,
               pmt_intern("interp-tx"),
               pmt_from_long(128));

  pmt_dict_set(usrp_dict,
               pmt_intern("decim-rx"),
               pmt_from_long(16));
  
  // Center frequency
  pmt_dict_set(usrp_dict,
               pmt_intern("rf-freq"),
               pmt_from_long((long)10e6));

  // Default is to use USRP considered '0' (incase of multiple)
  d_which_usrp = pmt_from_long(0);
  
  define_component("USRP-SERVER", "usrp_server", usrp_dict);
  
  connect("self", "us-tx0", "USRP-SERVER", "tx0");
  connect("self", "us-rx0", "USRP-SERVER", "rx0");
  connect("self", "us-cs", "USRP-SERVER", "cs");

  // Finally, enter the OPENING_USRP state by sending a request to open the
  // USRP.
  open_usrp();

}

// In the initialization state of the MAC layer we set default values for
// several functionalities.
void gmac::initialize_gmac()
{

  // The initial state is the INIT state.
  d_state = INIT_GMAC;

  // Set carrier sense to enabled by default with the specified threshold and
  // the deadline to 0 -- which is wait forever.
  set_carrier_sense(true, 25, 0, PMT_NIL);

  // Can now notify the application that we are initialized
  d_cs->send(s_response_gmac_initialized,
             pmt_list2(PMT_NIL, PMT_T));

  // The MAC enters an IDLE state where it waits for messages and dispatches
  // based on them
  enter_idle();
}

// Method for setting the carrier sense and an associated threshold which is
// written to a register on the FPGA, which it will read if the CS flag is set
// and perform carrier sense based on.
//
// We currently do not wait for the successful response for the write to
// register command, we assume it will succeed else the MAC must
void gmac::set_carrier_sense(bool toggle, long threshold, long deadline, pmt_t invocation)
{
  d_carrier_sense = toggle;

  // Only waste the bandwidth and processing of a C/S packet if needed
  if(threshold != d_cs_thresh) {
    d_us_tx->send(s_cmd_to_control_channel,    // C/S packet
               pmt_list2(invocation,           // invoc handle
                         pmt_list1(
                              pmt_list2(s_op_write_reg, 
                                        pmt_list2(
                                        pmt_from_long(REG_CS_THRESH), 
                                        pmt_from_long(threshold))))));
    d_cs_thresh = threshold;

    if(verbose)
      std::cout << "[GMAC] Changing CS threshold: " << d_cs_thresh << std::endl;
  }

  if(deadline != d_cs_deadline) {
    d_us_tx->send(s_cmd_to_control_channel,    // C/S packet
               pmt_list2(invocation,           // invoc handle
                         pmt_list1(
                              pmt_list2(s_op_write_reg, 
                                        pmt_list2(
                                        pmt_from_long(REG_CS_DEADLINE), 
                                        pmt_from_long(deadline))))));
    d_cs_deadline = deadline;

    if(verbose)
      std::cout << "[GMAC] Changing CS deadline: " << d_cs_deadline << std::endl;
  }

  if(verbose)
    std::cout << "[GMAC] Setting carrier sense to " << toggle << std::endl;
}

// The following sends a command to open the USRP, which will upload the
// specified RBF when creating the instance of the USRP server and set all other
// relevant parameters.
void gmac::open_usrp()
{
  d_state = OPENING_USRP;

  d_us_cs->send(s_cmd_open, pmt_list2(PMT_NIL, d_which_usrp));
  
  if(verbose)
    std::cout << "[GMAC] Opening USRP " 
              << d_which_usrp << std::endl;
}

// Before sending the close to the USRP we wait a couple seconds to let any data
// through the USB exit, else a bug in the driver will kick an error and cause
// an abnormal termination.
void gmac::close_usrp()
{
  d_state = CLOSING_USRP;

  sleep(2);

  d_us_cs->send(s_cmd_close, pmt_list1(PMT_NIL));
}

// RX and TX channels must be allocated so that the USRP server can
// properly share bandwidth across multiple USRPs.  No commands will be
// successful to the USRP through the USRP server on the TX or RX channels until
// a bandwidth allocation has been received.
void gmac::allocate_channels()
{
  d_state = ALLOCATING_CHANNELS;
  
  if(verbose)
    std::cout << "[GMAC] Sending channel allocation requests\n";

  long capacity = (long) 16e6;
  d_us_tx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
  d_us_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));

}

// Before closing the USRP connection, we deallocate our channels so that the
// capacity can be reused.
void gmac::close_channels()
{
  d_state = CLOSING_CHANNELS;

  d_us_tx->send(s_cmd_deallocate_channel, pmt_list2(PMT_NIL, d_us_tx_chan));
  d_us_rx->send(s_cmd_deallocate_channel, pmt_list2(PMT_NIL, d_us_rx_chan));

  if(verbose)
    std::cout << "[GMAC] Closing channels...\n";
}

// Used to enter the receiving state
void gmac::enter_receiving()
{
  d_us_rx->send(s_cmd_start_recv_raw_samples,
             pmt_list2(PMT_F,
                       d_us_rx_chan));

  if(verbose)
    std::cout << "[GMAC] Started RX sample stream\n";
}

// A simple idle state, nothing more to it.
void gmac::enter_idle()
{
  d_state = IDLE;
}

// Handles the transmission of a pkt from the application.  The invocation
// handle is passed on but a response is not given back to the application until
// the response is passed from usrp_server.  This ensures that the MAC passes
// back the success or failure.  Furthermore, the MAC could decide to retransmit
// on a failure based on the result of the packet transmission.
//
// This should eventually be connected to a physically layer rather than
// directly to usrp_server. (d_us_tx should be replaced with a different
// connection)
void gmac::handle_cmd_tx_pkt(pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t dst = pmt_nth(1, data);
  pmt_t samples = pmt_nth(2, data);
  pmt_t pkt_properties = pmt_nth(3, data);

  pmt_t us_tx_properties = pmt_make_dict();

  // Set the packet to be carrier sensed?
  if(carrier_sense_pkt(pkt_properties))
    pmt_dict_set(us_tx_properties,
                 pmt_intern("carrier-sense"),
                 PMT_T);

  pmt_t timestamp = pmt_from_long(0xffffffff);	// NOW

  // Construct the proper message for USRP server
  d_us_tx->send(s_cmd_xmit_raw_frame,
                pmt_list5(invocation_handle,
		                      d_us_tx_chan,
		                      samples, 
                          timestamp,
                          us_tx_properties));

  if(verbose && 0)
    std::cout << "[GMAC] Transmitted packet\n";
}

// Handles a response from the USRP server about the transmission of a frame,
// whether it was successful or failed.  This should eventually be replaced with
// a response from the PHY layer.  This is where a retransmit could be
// implemented.
void gmac::handle_response_xmit_raw_frame(pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);

  d_tx->send(s_response_tx_pkt,
             pmt_list2(invocation_handle,
                       status));
}

// This method determines whether carrier sense should be enabled based on two
// properties.  The first is the MAC setting, which the user can set to carrier
// sense packets by default or not.  The second is a per packet setting, which
// can be used to override the MAC setting for the given packet only.
bool gmac::carrier_sense_pkt(pmt_t pkt_properties) 
{
  // First we extract the per packet properties to check the per packet setting
  // if it exists
  if(pmt_is_dict(pkt_properties)) {

    if(pmt_t pkt_cs = pmt_dict_ref(pkt_properties,
                                   pmt_intern("carrier-sense"),
                                   PMT_NIL)) {
      // If the per packet property says true, enable carrier sense regardless
      // of the MAC setting
      if(pmt_eqv(pkt_cs, PMT_T))
        return true;
      // If the per packet setting says false, disable carrier sense regardless
      // of the MAC setting
      else if(pmt_eqv(pkt_cs, PMT_F))
        return false;
    }
  }

  // If we've hit this point, the packet properties did not state whether
  // carrier sense should be used or not, so we use the MAC setting
  if(d_carrier_sense)
    return true;
  else
    return false;

}

// This method is envoked by an incoming cmd-enable-carrier-sense signal on the
// C/S port.  It can be used to re-adjust the threshold or simply enabled
// carrier sense.  When a threshold is not provided, the MAC will use an
// averaging algorithm to determine the threshold (in the future).
void gmac::handle_cmd_carrier_sense_enable(pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t threshold = pmt_nth(1, data);
  pmt_t deadline = pmt_nth(2, data);
  long l_threshold, l_deadline;

  // FIXME: for now, if threshold is NIL, we do not change the threshold.
  // This should be replaced with an averaging algorithm
  if(pmt_eqv(threshold, PMT_NIL))
    l_threshold = d_cs_thresh;
  else
    l_threshold = pmt_to_long(threshold);

  // If the deadline is NIL, we do not change the value
  if(pmt_eqv(threshold, PMT_NIL))
    l_deadline = d_cs_deadline;
  else
    l_deadline = pmt_to_long(deadline);
  
  set_carrier_sense(true, l_threshold, l_deadline, invocation_handle);
}

// This method is called when an incoming disable carrier sense command is sent
// over the control status channel.  It so far does not ellicit a response, this
// needs to be added correctly.  It needs to wait for the response for the C/S
// packet from usrp_server.
void gmac::handle_cmd_carrier_sense_disable(pmt_t data) 
{
  pmt_t invocation_handle = pmt_nth(0, data);
  
  // We don't change the threshold, we leave it as is because the application
  // did not request that it changes, only to disable carrier sense.
  set_carrier_sense(false, d_cs_thresh, d_cs_deadline, invocation_handle);
}

// When the app requests that the threshold changes, the state of the carrier
// sense should not change.  If it was enabled, it should remain enabled.
// Likewise if it was disabled.  The deadline value should also remain
// unchanged.
void gmac::handle_cmd_carrier_sense_threshold(pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t threshold = pmt_nth(1, data);
  long l_threshold;

  // FIXME: for now, if threshold is NIL, we do not change the threshold.
  // This should be replaced with an averaging algorithm
  if(pmt_eqv(threshold, PMT_NIL))
    l_threshold = d_cs_thresh;
  else
    l_threshold = pmt_to_long(threshold);
  
  set_carrier_sense(d_carrier_sense, l_threshold, d_cs_deadline, invocation_handle);
}

// Ability to change the deadline using a C/S packet.  The state of all other
// carrier sense parameters should not change.
void gmac::handle_cmd_carrier_sense_deadline(pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t deadline = pmt_nth(1, data);
  long l_deadline;

  // If the deadline passed is NIL, do *not* change the value.
  if(pmt_eqv(deadline, PMT_NIL))
    l_deadline = d_cs_deadline;
  else
    l_deadline = pmt_to_long(deadline);
  
  set_carrier_sense(d_carrier_sense, d_cs_thresh, l_deadline, invocation_handle);
}

REGISTER_MBLOCK_CLASS(gmac);
