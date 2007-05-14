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
#include <usrp_server.h>
#include <iostream>
#include <usrp_inband_usb_packet.h>
#include <mb_class_registry.h>
#include <vector>

typedef usrp_inband_usb_packet transport_pkt;   // makes conversion to gigabit easy

// FIXME We should machine generate these by a simple preprocessor run over this file
//
// These are all the messages that we expect to receive.
//
// We "intern" these here (make them into symbols) so that our
// comparisions below are effectively pointer comparisons.

static pmt_t s_cmd_allocate_channel = pmt_intern("cmd-allocate-channel");
static pmt_t s_cmd_close = pmt_intern("cmd-close");
static pmt_t s_cmd_deallocate_channel = pmt_intern("cmd-deallocate-channel");
static pmt_t s_cmd_open = pmt_intern("cmd-open");
static pmt_t s_cmd_start_recv_raw_samples = pmt_intern("cmd-start-recv-raw-samples");
static pmt_t s_cmd_stop_recv_raw_samples = pmt_intern("cmd-stop-recv-raw-samples");
static pmt_t s_cmd_to_control_channel = pmt_intern("cmd-to-control-channel");
static pmt_t s_cmd_xmit_raw_frame  = pmt_intern("cmd-xmit-raw-frame");
static pmt_t s_cmd_max_capacity  = pmt_intern("cmd-max-capacity");
static pmt_t s_cmd_ntx_chan  = pmt_intern("cmd-ntx-chan");
static pmt_t s_cmd_nrx_chan  = pmt_intern("cmd-nrx-chan");
static pmt_t s_cmd_current_capacity_allocation  = pmt_intern("cmd-current-capacity-allocation");
static pmt_t s_response_allocate_channel = pmt_intern("response-allocate-channel");
static pmt_t s_response_close = pmt_intern("response-close");
static pmt_t s_response_deallocate_channel = pmt_intern("response-deallocate-channel");
static pmt_t s_response_from_control_channel = pmt_intern("response-from-control-channel");
static pmt_t s_response_open = pmt_intern("response-open");
static pmt_t s_response_recv_raw_samples = pmt_intern("response-recv-raw-samples");
static pmt_t s_response_xmit_raw_frame = pmt_intern("response-xmit-raw-frame");
static pmt_t s_response_max_capacity = pmt_intern("response-max-capacity");
static pmt_t s_response_ntx_chan = pmt_intern("response-ntx-chan");
static pmt_t s_response_nrx_chan = pmt_intern("response-nrx-chan");
static pmt_t s_response_current_capacity_allocation  = pmt_intern("response-current-capacity-allocation");

static std::string
str(long x)
{
  std::ostringstream s;
  s << x;
  return s.str();
}

usrp_server::usrp_server(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(rt, instance_name, user_arg)
{
  // define our ports

  // control & status port
  d_cs = define_port("cs", "usrp-server-cs", true, mb_port::EXTERNAL);	

  // ports
  //
  // (if/when we do replicated ports, these will be replaced by a
  //  single replicated port)
  for(int port=0; port < N_PORTS; port++) {
    d_tx.push_back(define_port("tx"+str(port), "usrp-tx", true, mb_port::EXTERNAL));
    d_rx.push_back(define_port("rx"+str(port), "usrp-rx", true, mb_port::EXTERNAL));
  }

  // FIXME ... initializing to 2 channels on each for now, eventually we should
  // query the FPGA to get these values
  d_ntx_chan = 2;
  d_nrx_chan = 2;

  // Initialize capacity on each channel to 0 and to no owner
  for(int chan=0; chan < d_ntx_chan; chan++) {
    d_chaninfo_tx[chan].assigned_capacity = 0;
    d_chaninfo_tx[chan].owner = PMT_NIL;
  }
  for(int chan=0; chan < d_nrx_chan; chan++) {
    d_chaninfo_rx[chan].assigned_capacity = 0;
    d_chaninfo_rx[chan].owner = PMT_NIL;
  }
}

usrp_server::~usrp_server()
{
}


void
usrp_server::initial_transition()
{
  // the initial transition
}

void
usrp_server::handle_message(mb_message_sptr msg)
{
  pmt_t event = msg->signal();		// the "name" of the message
  pmt_t port_id = msg->port_id();	// which port it came in on
  pmt_t data = msg->data();
  pmt_t metadata = msg->metadata();
  pmt_t invocation_handle;
  pmt_t reply_data;
  pmt_t status;

  if (1){
    std::cout << "[USRP_SERVER] event: " << event << std::endl;
    std::cout << "[USRP_SERVER] port_id: " << port_id << std::endl;
  }

  // It would be nice if this were all table driven, and we could
  // compute our state transition as f(current_state, port_id, signal)

  if (pmt_eq(port_id, d_cs->port_symbol())){	// message came in on our control/status port

    if (pmt_eq(event, s_cmd_open)){
      // extract args from data
      invocation_handle = pmt_nth(0, data);
      long which_usrp = pmt_to_long(pmt_nth(1, data));	// integer usrp id, usually 0
      
      // Do the right thing....
      // build a reply
      (void) which_usrp;	// avoid unused warning

      // if everything OK
      status = PMT_T;
      reply_data = pmt_list2(invocation_handle, status);

      //  ...and send it
      d_cs->send(s_response_open, reply_data);
      return;
    }
    else if (pmt_eq(event, s_cmd_close)){
      // ...
    }
    else if (pmt_eq(event, s_cmd_max_capacity)) {
      invocation_handle = pmt_nth(0, data);
      reply_data = pmt_list2(invocation_handle, pmt_from_long(max_capacity()));
      d_cs->send(s_response_max_capacity, reply_data);
      return;
    }
    else if (pmt_eq(event, s_cmd_ntx_chan)) {
      invocation_handle = pmt_nth(0, data);
      reply_data = pmt_list2(invocation_handle, pmt_from_long(d_ntx_chan));
      d_cs->send(s_response_ntx_chan, reply_data);
    }
    else if (pmt_eq(event, s_cmd_nrx_chan)) {
      invocation_handle = pmt_nth(0, data);
      reply_data = pmt_list2(invocation_handle, pmt_from_long(d_nrx_chan));
      d_cs->send(s_response_nrx_chan, reply_data);
    }
    else if (pmt_eq(event, s_cmd_current_capacity_allocation)) {
      invocation_handle = pmt_nth(0, data);
      reply_data = pmt_list2(invocation_handle, pmt_from_long(current_capacity_allocation()));
      d_cs->send(s_response_current_capacity_allocation, reply_data);
    }
    goto unhandled;
  }

  if (pmt_eq(event, s_cmd_allocate_channel)){
    handle_cmd_allocate_channel(port_id, data);
    return;
  }

  if (pmt_eq(event, s_cmd_deallocate_channel)) {
    handle_cmd_deallocate_channel(port_id, data);
    return;
  }
    
  if (pmt_eq(event, s_cmd_xmit_raw_frame)){
    handle_cmd_xmit_raw_frame(data);
    return;
  }

 unhandled:
  std::cout << "[USRP_SERVER] unhandled msg: " << msg << std::endl;
}

// Return -1 if it is not an RX port, or an index
int usrp_server::tx_port_index(pmt_t port_id) {

  for(int i=0; i < (int) d_tx.size(); i++) 
    if(pmt_eq(d_tx[i]->port_symbol(), port_id))
      return i;

  return -1;
}

// Return -1 if it is not an RX port, or an index
int usrp_server::rx_port_index(pmt_t port_id) {
  
  for(int i=0; i < (int) d_rx.size(); i++) 
    if(pmt_eq(d_rx[i]->port_symbol(), port_id))
      return i;

  return -1;
}

// Go through all TX and RX channels, sum up the assigned capacity
// and return it
long usrp_server::current_capacity_allocation() {
  long capacity = 0;

  for(int chan=0; chan < d_ntx_chan; chan++) 
    capacity += d_chaninfo_tx[chan].assigned_capacity;

  for(int chan=0; chan < d_nrx_chan; chan++)
    capacity += d_chaninfo_rx[chan].assigned_capacity;

  return capacity;
}
    
void usrp_server::handle_cmd_allocate_channel(pmt_t port_id, pmt_t data) {

  pmt_t invocation_handle = pmt_nth(0, data);
  long rqstd_capacity = pmt_to_long(pmt_nth(1, data));
  long chan, port;
  pmt_t reply_data;

  // If it's a TX port, allocate on a free channel, else check if it's a RX port
  // and allocate.
  if((port = tx_port_index(port_id)) != -1) {

    // Check capacity exists
    if((D_USB_CAPACITY - current_capacity_allocation()) < rqstd_capacity) {
      reply_data = pmt_list3(invocation_handle, pmt_from_long(RQSTD_CAPACITY_UNAVAIL), PMT_NIL);  // no capacity available
      d_tx[port]->send(s_response_allocate_channel, reply_data);
      return;
    }

    // Find a free channel, assign the capacity and respond
    for(chan=0; chan < d_ntx_chan; chan++) {
      if(d_chaninfo_tx[chan].owner == PMT_NIL) {
        d_chaninfo_tx[chan].owner = port_id;
        d_chaninfo_tx[chan].assigned_capacity = rqstd_capacity;
        reply_data = pmt_list3(invocation_handle, PMT_T, pmt_from_long(chan));
        d_tx[port]->send(s_response_allocate_channel, reply_data);
        return;
      }
    }

    std::cout << "[USRP_SERVER] Couldnt find a TX chan\n";

    reply_data = pmt_list3(invocation_handle, pmt_from_long(CHANNEL_UNAVAIL), PMT_NIL);  // no free TX chan found
    d_tx[port]->send(s_response_allocate_channel, reply_data);
    return;
  }
  
  // Repeat the same process on the RX side if the port was not determined to be TX
  if((port = rx_port_index(port_id)) != -1) {
    
    if((D_USB_CAPACITY - current_capacity_allocation()) < rqstd_capacity) {
      reply_data = pmt_list3(invocation_handle, pmt_from_long(RQSTD_CAPACITY_UNAVAIL), PMT_NIL);  // no capacity available
      d_rx[port]->send(s_response_allocate_channel, reply_data);
      return;
    }

    for(chan=0; chan < d_nrx_chan; chan++) {
      if(d_chaninfo_rx[chan].owner == PMT_NIL) {
        d_chaninfo_rx[chan].owner = port_id;
        d_chaninfo_rx[chan].assigned_capacity = rqstd_capacity;
        reply_data = pmt_list3(invocation_handle, PMT_T, pmt_from_long(chan));
        d_rx[port]->send(s_response_allocate_channel, reply_data);
        return;
      }
    }

    std::cout << "[USRP_SERVER] Couldnt find a RX chan\n";
    reply_data = pmt_list3(invocation_handle, pmt_from_long(CHANNEL_UNAVAIL), PMT_NIL);  // no free RX chan found
    d_rx[port]->send(s_response_allocate_channel, reply_data);
    return;
  }
}

// Check the port type and deallocate assigned capacity based on this, ensuring
// that the owner of the method invocation is the owner of the port and that
// the channel number is valid.
void usrp_server::handle_cmd_deallocate_channel(pmt_t port_id, pmt_t data) {

  pmt_t invocation_handle = pmt_nth(0, data); 
  long channel = pmt_to_long(pmt_nth(1, data));
  long port;
  pmt_t reply_data;
  
  // Check that the channel number is valid, and that the calling port is the owner
  // of the channel, and if so remove the assigned capacity.
  if((port = tx_port_index(port_id)) != -1) {
  
    if(channel >= d_ntx_chan) {
      reply_data = pmt_list2(invocation_handle, pmt_from_long(CHANNEL_INVALID));   // not a legit channel number
      d_tx[port]->send(s_response_deallocate_channel, reply_data);
      return;
    }

    if(d_chaninfo_tx[channel].owner != port_id) {
      reply_data = pmt_list2(invocation_handle, pmt_from_long(PERMISSION_DENIED));   // not the owner of the port
      d_tx[port]->send(s_response_deallocate_channel, reply_data);
      return;
    }

    d_chaninfo_tx[channel].assigned_capacity = 0;
    d_chaninfo_tx[channel].owner = PMT_NIL;

    reply_data = pmt_list2(invocation_handle, PMT_T);
    d_tx[port]->send(s_response_deallocate_channel, reply_data);
    return;
  }

  // Repeated process on the RX side
  if((port = rx_port_index(port_id)) != -1) {
  
    if(channel >= d_nrx_chan) {
      reply_data = pmt_list2(invocation_handle, pmt_from_long(CHANNEL_INVALID));   // not a legit channel number
      d_rx[port]->send(s_response_deallocate_channel, reply_data);
      return;
    }

    if(d_chaninfo_rx[channel].owner != port_id) {
      reply_data = pmt_list2(invocation_handle, pmt_from_long(PERMISSION_DENIED));   // not the owner of the port
      d_rx[port]->send(s_response_deallocate_channel, reply_data);
      return;
    }

    d_chaninfo_rx[channel].assigned_capacity = 0;
    d_chaninfo_rx[channel].owner = PMT_NIL;

    reply_data = pmt_list2(invocation_handle, PMT_T);
    d_rx[port]->send(s_response_deallocate_channel, reply_data);
    return;
  }

}

void usrp_server::handle_cmd_xmit_raw_frame(pmt_t data) {

  size_t n_bytes, psize;
  long max_payload_len = transport_pkt::max_payload();

  pmt_t invocation_handle = pmt_nth(0, data);
  long channel = pmt_to_long(pmt_nth(1, data));
  const void *samples = pmt_uniform_vector_elements(pmt_nth(2, data), n_bytes);
  long timestamp = pmt_to_long(pmt_nth(3, data));

  // Determine the number of packets to allocate contiguous memory for bursting over the
  // USB and get a pointer to the memory to be used in building the packets
  long n_packets = static_cast<long>(std::ceil(n_bytes / (double)max_payload_len));
  pmt_t v_packets = pmt_make_u8vector(sizeof(transport_pkt) * n_packets, 0);

  transport_pkt *pkts =
    (transport_pkt *) pmt_u8vector_writeable_elements(v_packets, psize);

  for(int n=0; n < n_packets; n++) {

    long payload_len = std::min((long)(n_bytes-(n*max_payload_len)), (long)max_payload_len);
  
    if(n == 0) { // first packet gets start of burst flag and timestamp
      pkts[n].set_header(pkts[n].FL_START_OF_BURST, channel, 0, payload_len);
      pkts[n].set_timestamp(timestamp);
    } else {
      pkts[n].set_header(0, channel, 0, payload_len);
      pkts[n].set_timestamp(0xffffffff);
    }

    memcpy(pkts[n].payload(), (uint8_t *)samples+(max_payload_len * n), payload_len);
  }

  pkts[n_packets-1].set_end_of_burst();   // set the last packet's end of burst

  // interface with the USRP to send the USB packet, since the memory is
  // contiguous, this should be a serious of memory copies to the bus, each being
  // USB_PKT_SIZE * MAX_PACKET_BURST bytes worth of data (given a full burst)
}

REGISTER_MBLOCK_CLASS(usrp_server);
