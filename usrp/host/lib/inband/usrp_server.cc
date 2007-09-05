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
#include <usrp_server.h>
#include <iostream>
#include <usrp_inband_usb_packet.h>
#include <mb_class_registry.h>
#include <vector>
#include <usrp_usb_interface.h>

#include <symbols_usrp_server_cs.h>
#include <symbols_usrp_channel.h>
#include <symbols_usrp_tx.h>
#include <symbols_usrp_rx.h>
#include <symbols_usrp_low_level_cs.h>
#include <symbols_usrp_interface_cs.h>
static pmt_t s_shutdown = pmt_intern("%shutdown");

typedef usrp_inband_usb_packet transport_pkt;   // makes conversion to gigabit easy

const static bool verbose = false;

static std::string
str(long x)
{
  std::ostringstream s;
  s << x;
  return s.str();
}

usrp_server::usrp_server(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(rt, instance_name, user_arg),
  d_fake_rx(false)
{

  // Dictionary for arguments to all of the components
  pmt_t usrp_dict = user_arg;
  
  // control & status port
  d_cs = define_port("cs", "usrp-server-cs", true, mb_port::EXTERNAL);	
  d_cs_usrp = define_port("cs_usrp", "usrp-interface-cs", false, mb_port::INTERNAL);	

  // ports
  //
  // (if/when we do replicated ports, these will be replaced by a
  //  single replicated port)
  for(int port=0; port < N_PORTS; port++) {

    d_tx.push_back(define_port("tx"+str(port), 
                               "usrp-tx", 
                               true, 
                               mb_port::EXTERNAL));

    d_rx.push_back(define_port("rx"+str(port), 
                               "usrp-rx", 
                               true, 
                               mb_port::EXTERNAL));
  }

  define_component("usrp", "usrp_usb_interface", usrp_dict);
  connect("self", "cs_usrp", "usrp", "cs");

  d_defer=false;
  d_opened=false;

  // FIXME: needs to be returned from open, if we want to use this
  d_nrx_chan = 2;
  d_ntx_chan = 2;

  // Initialize capacity on each channel to 0 and to no owner
  // Also initialize the USRP standard tx/rx pointers to NULL
  for(int chan=0; chan < d_ntx_chan; chan++)
    d_chaninfo_tx.push_back(channel_info());

  for(int chan=0; chan < d_nrx_chan; chan++)
    d_chaninfo_rx.push_back(channel_info());

  d_rx_chan_mask = 0;

  for(int i=0; i < D_MAX_RID; i++) 
    d_rids.push_back(rid_info());

  //d_fake_rx=true;
}

void
usrp_server::reset_channels()
{

  for(int chan=0; chan < d_ntx_chan; chan++) {
    d_chaninfo_tx[chan].assigned_capacity = 0;
    d_chaninfo_tx[chan].owner = PMT_NIL;
  }

  for(int chan=0; chan < d_nrx_chan; chan++) {
    d_chaninfo_rx[chan].assigned_capacity = 0;
    d_chaninfo_rx[chan].owner = PMT_NIL;
  }

  d_rx_chan_mask = 0;
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
  pmt_t invocation_handle;
  pmt_t metadata = msg->metadata();
  pmt_t status;

  long port;

  if (pmt_eq(event, s_shutdown))	// ignore (for now)
    return;

  invocation_handle = pmt_nth(0, data);

  if (0){
    std::cout << "[USRP_SERVER] event: " << event << std::endl;
    std::cout << "[USRP_SERVER] port_id: " << port_id << std::endl;
  }

  // It would be nice if this were all table driven, and we could compute our
  // state transition as f(current_state, port_id, signal)
  
  // A message from the USRP CS, which should *only* be responses
  //
  // It is important that this set come before checking messages of any other
  // components.  This is since we always want to listen to the low level USRP
  // server, even if we aren't initialized we are waiting for responses to
  // become initialized.  Likewise, after the usrp_server is "closed", we still
  // want to pass responses back from the low level.

  //---------------- USRP RESPONSE ---------------//
  if (pmt_eq(port_id, d_cs_usrp->port_symbol())) { 
    
    //-------------- USRP OPEN ------------------//
    if(pmt_eq(event, s_response_usrp_open)) {
      // pass the response back over the regular CS port
      pmt_t status = pmt_nth(1, data);
      d_cs->send(s_response_open, pmt_list2(invocation_handle, status));

      if(pmt_eqv(status,PMT_T)) {
        d_opened = true;
        d_defer = false;
        recall_defer_queue();
      }

      return;
    }
    //------------- USRP CLOSE -------------------//
    else if (pmt_eq(event, s_response_usrp_close)) {
      pmt_t status = pmt_nth(1, data);
      d_cs->send(s_response_close, pmt_list2(invocation_handle, status));

      if(pmt_eqv(status,PMT_T)) {
        d_opened = false;
        d_defer = false;
        reset_channels();
        recall_defer_queue();
      }
      
      return;
    }
    //--------------- USRP WRITE --------------//
    else if (pmt_eq(event, s_response_usrp_write)) {
      
      pmt_t status = pmt_nth(1, data);
      long channel = pmt_to_long(pmt_nth(2, data));
      long port;

      // Do not report back responses if they were generated from a
      // command packet
      if(channel == 0x1f)
        return;

      // Find the port through the owner of the channel
      if((port = tx_port_index(d_chaninfo_tx[channel].owner)) !=-1 )
        d_tx[port]->send(s_response_xmit_raw_frame, 
                         pmt_list2(invocation_handle, status));
      return;
    }
    //--------------- USRP READ ---------------//
    else if (pmt_eq(event, s_response_usrp_read)) {

      pmt_t status = pmt_nth(1, data);

      if(!pmt_eqv(status, PMT_T)) {
        std::cerr << "[USRP_SERVER] Error receiving packet\n";
        return;
      }
      else {
        handle_response_usrp_read(data);
        return;
      }
    }

    goto unhandled;
  }

  // Checking for defer on all other messages
  if(d_defer) {
    if (verbose)
      std::cout << "[USRP_SERVER] Received msg while deferring (" 
                << msg->signal() << ")\n";
    d_defer_queue.push(msg);
    return;
  }
  
  //--------- CONTROL / STATUS ------------//
  if (pmt_eq(port_id, d_cs->port_symbol())){
    
    //----------- OPEN -----------//
    if (pmt_eq(event, s_cmd_open)){

      // Reject if already open
      if(d_opened) {
        d_cs->send(s_response_open, pmt_list2(invocation_handle, s_err_usrp_already_opened));
        return;
      }

      // the parameters are the same to the low level interface, so we just pass 'data' along
      d_cs_usrp->send(s_cmd_usrp_open, data);

      d_defer = true;
      
      return;
    }
    //---------- CLOSE -----------//
    else if (pmt_eq(event, s_cmd_close)){
      
      if(!d_opened) { 
        d_cs->send(s_response_close, pmt_list2(invocation_handle, s_err_usrp_already_closed));
        return;
      }
      
      d_defer = true;
      d_cs_usrp->send(s_cmd_usrp_close, pmt_list1(invocation_handle));

      return;
    }
    //---------- MAX CAPACITY ----------//
    else if (pmt_eq(event, s_cmd_max_capacity)) {
      
      if(!d_opened) { 
        d_cs->send(s_response_max_capacity, 
                   pmt_list3(invocation_handle, s_err_usrp_not_opened, pmt_from_long(0)));
        return;
      }

      d_cs->send(s_response_max_capacity, 
                 pmt_list3(invocation_handle, 
                           PMT_T, 
                           pmt_from_long(max_capacity())));
      return;
    }
    //---------- NTX CHAN --------------//
    else if (pmt_eq(event, s_cmd_ntx_chan)) {

      if(!d_opened) { 
        d_cs->send(s_response_ntx_chan, 
                   pmt_list3(invocation_handle, s_err_usrp_not_opened, pmt_from_long(0)));
        return;
      }

      d_cs->send(s_response_ntx_chan, 
                 pmt_list3(invocation_handle, 
                           PMT_T, 
                           pmt_from_long(d_ntx_chan)));
      return;
    }
    //---------- NRX CHAN -----------//
    else if (pmt_eq(event, s_cmd_nrx_chan)) {

      if(!d_opened) { 
        d_cs->send(s_response_nrx_chan, 
                   pmt_list3(invocation_handle, s_err_usrp_not_opened, pmt_from_long(0)));
        return;
      }

      d_cs->send(s_response_nrx_chan, 
                 pmt_list3(invocation_handle, 
                           PMT_T, 
                           pmt_from_long(d_nrx_chan)));
      return;
    }	
    //--------- ALLOCATION? -----------//
    else if (pmt_eq(event, s_cmd_current_capacity_allocation)) {
      
      if(!d_opened) { 
        d_cs->send(s_response_current_capacity_allocation, 
                   pmt_list3(invocation_handle, 
                             s_err_usrp_not_opened, 
                             pmt_from_long(0)));
        return;
      }
      
      d_cs->send(s_response_current_capacity_allocation, 
                 pmt_list3(invocation_handle, 
                           PMT_T, 
                           pmt_from_long(current_capacity_allocation())));
      return;
    }
    goto unhandled;
  }
  
  //-------------- TX ---------------//
  if ((port = tx_port_index(port_id)) != -1) {
    
    //------------ ALLOCATE (TX) ----------------//
    if (pmt_eq(event, s_cmd_allocate_channel)){
      
      if(!d_opened) { 
        d_tx[port]->send(s_response_allocate_channel, 
                          pmt_list3(invocation_handle, 
                                    s_err_usrp_not_opened, 
                                    pmt_from_long(0)));
        return;
      }
        
      handle_cmd_allocate_channel(d_tx[port], d_chaninfo_tx, data);
      return;
    }
  
    //----------- DEALLOCATE (TX) ---------------//
    if (pmt_eq(event, s_cmd_deallocate_channel)) {
    
      if(!d_opened) {
        d_tx[port]->send(s_response_deallocate_channel, 
                         pmt_list3(invocation_handle, 
                                   s_err_usrp_not_opened, 
                                   pmt_from_long(0)));
        return;
      }

      handle_cmd_deallocate_channel(d_tx[port], d_chaninfo_tx, data);
      return;
    }
  
    //-------------- XMIT RAW FRAME -----------------/
    if (pmt_eq(event, s_cmd_xmit_raw_frame)){

      if(!d_opened) { 
        d_tx[port]->send(s_response_xmit_raw_frame, 
                         pmt_list2(invocation_handle, s_err_usrp_not_opened));
        return;
      }
      
      handle_cmd_xmit_raw_frame(d_tx[port], d_chaninfo_tx, data);
      return;
    }
    
    //-------------- CONTROL PACKET -----------------/
    if (pmt_eq(event, s_cmd_to_control_channel)) {
      
      if(!d_opened) { 
        d_tx[port]->send(s_response_xmit_raw_frame, 
                         pmt_list2(invocation_handle, s_err_usrp_not_opened));
        return;
      }
      
      handle_cmd_to_control_channel(d_tx[port], d_chaninfo_tx, data);
      return;

    }

    goto unhandled;
  }

  //-------------- RX ---------------//
  if ((port = rx_port_index(port_id)) != -1) {
    
    //------------ ALLOCATE (RX) ----------------//
    if (pmt_eq(event, s_cmd_allocate_channel)) {
      
      if(!d_opened) { 
        d_rx[port]->send(s_response_allocate_channel, 
                          pmt_list3(invocation_handle, 
                                    s_err_usrp_not_opened, 
                                    pmt_from_long(0)));
        return;
      }
        
      handle_cmd_allocate_channel(d_rx[port], d_chaninfo_rx, data);
      return;
    }
  
    //----------- DEALLOCATE (RX) ---------------//
    if (pmt_eq(event, s_cmd_deallocate_channel)) {
    
      if(!d_opened) {
        d_rx[port]->send(s_response_deallocate_channel, 
                         pmt_list3(invocation_handle, 
                                   s_err_usrp_not_opened, 
                                   pmt_from_long(0)));
        return;
      }

      handle_cmd_deallocate_channel(d_rx[port], d_chaninfo_rx, data);
      return;
    }
  
    //-------------- START RECV ----------------//
    if (pmt_eq(event, s_cmd_start_recv_raw_samples)) {
    
      if(!d_opened) {
        d_rx[port]->send(s_response_recv_raw_samples,
                         pmt_list2(invocation_handle, s_err_usrp_not_opened));
        return;
      }

      handle_cmd_start_recv_raw_samples(d_rx[port], d_chaninfo_rx, data);
      return;
    }
    
    //-------------- STOP RECV ----------------//
    if (pmt_eq(event, s_cmd_stop_recv_raw_samples)) {
    
      if(!d_opened) 
        return;

      // FIX ME : no response for stopping? even if error? (permissions)
      handle_cmd_stop_recv_raw_samples(d_rx[port], d_chaninfo_rx, data);

      return;
    }

    goto unhandled;
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
    
void 
usrp_server::handle_cmd_allocate_channel(
                                mb_port_sptr port, 
                                std::vector<struct channel_info> &chan_info,
                                pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  long rqstd_capacity = pmt_to_long(pmt_nth(1, data));
  long chan;

  // Check capacity exists
  if((D_USB_CAPACITY - current_capacity_allocation()) < rqstd_capacity) {

    // no capacity available
    port->send(s_response_allocate_channel, 
               pmt_list3(invocation_handle, 
                         s_err_requested_capacity_unavailable, 
                         PMT_NIL));
    return;
  }

  // Find a free channel, assign the capacity and respond
  for(chan=0; chan < (long)chan_info.size(); chan++) {

    if(verbose)
      std::cout << "[USRP_SERVER] Checking chan: " << chan
                << " owner " << chan_info[chan].owner
                << " size " << chan_info.size()
                << std::endl;

    if(chan_info[chan].owner == PMT_NIL) {
  
      chan_info[chan].owner = port->port_symbol();
      chan_info[chan].assigned_capacity = rqstd_capacity;
      
      port->send(s_response_allocate_channel, 
                 pmt_list3(invocation_handle, 
                           PMT_T, 
                           pmt_from_long(chan)));

      if(verbose)
        std::cout << "[USRP_SERVER] Assigning channel: " << chan 
                  << " to " << chan_info[chan].owner
                  << std::endl;
      return;
    }
  
  }

  if (verbose)
    std::cout << "[USRP_SERVER] Couldnt find a TX chan\n";

  // no free TX chan found
  port->send(s_response_allocate_channel, 
             pmt_list3(invocation_handle, 
                       s_err_channel_unavailable, 
                       PMT_NIL));
  return;
}

// Check the port type and deallocate assigned capacity based on this, ensuring
// that the owner of the method invocation is the owner of the port and that the
// channel number is valid.
void 
usrp_server::handle_cmd_deallocate_channel(
                              mb_port_sptr port, 
                              std::vector<struct channel_info> &chan_info, 
                              pmt_t data)
{

  pmt_t invocation_handle = pmt_nth(0, data); 
  long channel = pmt_to_long(pmt_nth(1, data));

  // Ensure the channel is valid and the caller owns the port
  if(!check_valid(port, channel, chan_info,
                  pmt_list2(s_response_deallocate_channel, invocation_handle)))
    return;
  
  chan_info[channel].assigned_capacity = 0;
  chan_info[channel].owner = PMT_NIL;

  port->send(s_response_deallocate_channel, 
             pmt_list2(invocation_handle, 
                       PMT_T));
  return;
}

void usrp_server::handle_cmd_xmit_raw_frame(mb_port_sptr port, std::vector<struct channel_info> &chan_info, pmt_t data) {

  size_t n_bytes, psize;
  long max_payload_len = transport_pkt::max_payload();

  pmt_t invocation_handle = pmt_nth(0, data);
  long channel = pmt_to_long(pmt_nth(1, data));
  const void *samples = pmt_uniform_vector_elements(pmt_nth(2, data), n_bytes);
  long timestamp = pmt_to_long(pmt_nth(3, data));
  
  // Ensure the channel is valid and the caller owns the port
  if(!check_valid(port, channel, chan_info,
                  pmt_list2(s_response_xmit_raw_frame, invocation_handle)))
    return;
  
  // Determine the number of packets to allocate contiguous memory for
  // bursting over the USB and get a pointer to the memory to be used in
  // building the packets
  long n_packets = 
    static_cast<long>(std::ceil(n_bytes / (double)max_payload_len));

  pmt_t v_packets = pmt_make_u8vector(sizeof(transport_pkt) * n_packets, 0);

  transport_pkt *pkts =
    (transport_pkt *) pmt_u8vector_writeable_elements(v_packets, psize);

  for(int n=0; n < n_packets; n++) {

    long payload_len = 
      std::min((long)(n_bytes-(n*max_payload_len)), (long)max_payload_len);
  
    if(n == 0) { // first packet gets start of burst flag and timestamp
      pkts[n].set_header(pkts[n].FL_START_OF_BURST, channel, 0, payload_len);
      pkts[n].set_timestamp(timestamp);
    } else {
      pkts[n].set_header(0, channel, 0, payload_len);
      pkts[n].set_timestamp(0xffffffff);
    }

    memcpy(pkts[n].payload(), 
           (uint8_t *)samples+(max_payload_len * n), 
           payload_len);
  
  }


  pkts[n_packets-1].set_end_of_burst(); // set the last packet's end of burst

  if (verbose)
    std::cout << "[USRP_SERVER] Received raw frame invocation: " 
              << invocation_handle << std::endl;
    
  // The actual response to the write will be generated by a
  // s_response_usrp_write
  d_cs_usrp->send(s_cmd_usrp_write, 
                  pmt_list3(invocation_handle, 
                            pmt_from_long(channel), 
                            v_packets));

  return;
}

void usrp_server::handle_cmd_to_control_channel(mb_port_sptr port, std::vector<struct channel_info> &chan_info, pmt_t data) 
{

  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t subpackets = pmt_nth(1, data);

  long n_subpkts = pmt_length(subpackets);
  long curr_subpkt = 0;

  size_t psize;
  long payload_len = 0;
  long channel = 0x1f;

  // The design of the following code is optimized for simplicity, not
  // performance.  To performance optimize this code, the total size in bytes
  // needed for all of the CS packets is needed to allocate contiguous memory
  // which contains the USB packets for bursting over the bus.  However to do
  // this the packets subpackets would need to be parsed twice and their sizes
  // would need to be determined.
  //
  // The approach taken is to keep parsing the subpackets and putting them in to
  // USB packets.  Once the USB packet is full, a write is sent for it and
  // another packet is created.
  //
  // The subpacket creation methods will return false if the subpacket will not
  // fit in to the current USB packet.  In these cases a new USB packet is
  // created and the old is sent.
  
  new_packet:
    // This code needs to become "smart" and only make a new packet when full
    pmt_t v_packet = pmt_make_u8vector(sizeof(transport_pkt), 0);
    transport_pkt *pkt = (transport_pkt *) pmt_u8vector_writeable_elements(v_packet, psize);
    payload_len = 0;
    
    pkt->set_header(0, channel, 0, payload_len);
    pkt->set_timestamp(0xffffffff);

  while(curr_subpkt < n_subpkts) {

    pmt_t subp = pmt_nth(curr_subpkt, subpackets);
    pmt_t subp_cmd = pmt_nth(0, subp);
    pmt_t subp_data = pmt_nth(1, subp);

    //--------- PING FIXED --------------//
    if(pmt_eq(subp_cmd, s_op_ping_fixed)) {

      long urid     = pmt_to_long(pmt_nth(0, subp_data));
      long pingval  = pmt_to_long(pmt_nth(1, subp_data));

      // USRP server sets request ID's to keep track of which application gets
      // what response back.  To allow a full 6-bits for an RID to the user, we
      // keep a mapping and replace the RID's as the packets go in and out.  If
      // there are no RID's available, the command is thrown away silently. 
      long srid;
      if((srid = next_rid()) == -1)
        goto subpkt_bail;

      // We use a vector to store the owner of the ping request and will use it
      // to send the request on any RX port they own. 
      d_rids[srid].owner = port->port_symbol();
      d_rids[srid].user_rid = urid;
        
      // Adds a ping after the previous command in the pkt
      if(!pkt->cs_ping(srid, pingval))
      {
        d_cs_usrp->send(s_cmd_usrp_write, 
                        pmt_list3(invocation_handle, 
                                  pmt_from_long(channel), 
                                  v_packet));

        // Return the RID
        d_rids[srid].owner = PMT_NIL;

        goto new_packet;
      }

      if(verbose)
        std::cout << "[USRP_SERVER] Received ping command request"
                  << " assigning RID " << srid << std::endl;

    }
  
    //----------- WRITE REG ---------------//
    if(pmt_eq(subp_cmd, s_op_write_reg)) {
      
      long reg_num = pmt_to_long(pmt_nth(0, subp_data));
      long val = pmt_to_long(pmt_nth(1, subp_data));

      if(!pkt->cs_write_reg(reg_num, val))
      {
        d_cs_usrp->send(s_cmd_usrp_write, 
                        pmt_list3(invocation_handle, 
                                  pmt_from_long(channel), 
                                  v_packet));
        
        goto new_packet;
      }
      
      if(verbose)
        std::cout << "[USRP_SERVER] Received write register request "
                  << "("
                  << "Reg: " << reg_num << ", "
                  << "Val: " << val
                  << ")\n";
    }
    
    //------- WRITE REG MASKED ----------//
    if(pmt_eq(subp_cmd, s_op_write_reg_masked)) {
      
      long reg_num = pmt_to_long(pmt_nth(0, subp_data));
      long val = pmt_to_long(pmt_nth(1, subp_data));
      long mask = pmt_to_long(pmt_nth(2, subp_data));

      if(!pkt->cs_write_reg_masked(reg_num, val, mask))
      {
        d_cs_usrp->send(s_cmd_usrp_write, 
                        pmt_list3(invocation_handle, 
                                  pmt_from_long(channel), 
                                  v_packet));
        
        goto new_packet;
      }
      
      if(verbose)
        std::cout << "[USRP_SERVER] Received write register masked request\n";
    }
    
    //------------ READ REG --------------//
    if(pmt_eq(subp_cmd, s_op_read_reg)) {
      
      long urid     = pmt_to_long(pmt_nth(0, subp_data));
      long reg_num  = pmt_to_long(pmt_nth(1, subp_data));

      long srid;
      if((srid = next_rid()) == -1)
        goto subpkt_bail;

      d_rids[srid].owner = port->port_symbol();
      d_rids[srid].user_rid = urid;

      if(!pkt->cs_read_reg(srid, reg_num))
      {
        d_cs_usrp->send(s_cmd_usrp_write, 
                        pmt_list3(invocation_handle, 
                                  pmt_from_long(channel), 
                                  v_packet));

        // Return the rid
        d_rids[srid].owner = PMT_NIL;
        
        goto new_packet;
      }
      
      if(verbose)
        std::cout << "[USRP_SERVER] Received read register request"
                  << " assigning RID " << srid << std::endl;
    }
    
    //------------ DELAY --------------//
    if(pmt_eq(subp_cmd, s_op_delay)) {

      long ticks = pmt_to_long(pmt_nth(0, subp_data));

      if(!pkt->cs_delay(ticks))
      {
        d_cs_usrp->send(s_cmd_usrp_write, 
                        pmt_list3(invocation_handle, 
                                  pmt_from_long(channel), 
                                  v_packet));
        
        goto new_packet;
      }
      
      if(verbose)
        std::cout << "[USRP_SERVER] Received delay request of "
                  << ticks << " ticks\n";
    }

    //--------- I2C WRITE -----------//
    // FIXME: could check that byte count does not exceed 2^8 which
    // is the max length in the subpacket for # of bytes to read.
    if(pmt_eq(subp_cmd, s_op_i2c_write)) {
      
      long i2c_addr = pmt_to_long(pmt_nth(0, subp_data));
      pmt_t data = pmt_nth(1, subp_data);

      // Get a readable address to the data which also gives us the length
      size_t data_len;
      uint8_t *i2c_data = (uint8_t *) pmt_u8vector_writeable_elements(data, data_len);

      // Make the USB packet
      if(!pkt->cs_i2c_write(i2c_addr, i2c_data, data_len))
      {
        d_cs_usrp->send(s_cmd_usrp_write, 
                        pmt_list3(invocation_handle, 
                                  pmt_from_long(channel), 
                                  v_packet));
        
        goto new_packet;
      }
      
      if(verbose)
        std::cout << "[USRP_SERVER] Received I2C write\n";
    }
  
    //----------- I2C Read -------------//
    if(pmt_eq(subp_cmd, s_op_i2c_read)) {
      
      long urid       = pmt_to_long(pmt_nth(0, subp_data));
      long i2c_addr   = pmt_to_long(pmt_nth(1, subp_data));
      long i2c_bytes  = pmt_to_long(pmt_nth(2, subp_data));

      long srid;
      if((srid = next_rid()) == -1)
        goto subpkt_bail;
      
      d_rids[srid].owner = port->port_symbol();
      d_rids[srid].user_rid = urid;

      if(!pkt->cs_i2c_read(srid, i2c_addr, i2c_bytes))
      {
        
        d_cs_usrp->send(s_cmd_usrp_write, 
                        pmt_list3(invocation_handle, 
                                  pmt_from_long(channel), 
                                  v_packet));

        d_rids[srid].owner = PMT_NIL;

        goto new_packet;
      }
      
      if(verbose)
        std::cout << "[USRP_SERVER] Received I2C read\n";
    }
    
    //--------- SPI WRITE -----------//
    if(pmt_eq(subp_cmd, s_op_spi_write)) {
      
      long enables = pmt_to_long(pmt_nth(0, subp_data));
      long format = pmt_to_long(pmt_nth(1, subp_data));
      long opt = pmt_to_long(pmt_nth(2, subp_data));
      pmt_t data = pmt_nth(3, subp_data);

      // Get a readable address to the data which also gives us the length
      size_t data_len;
      uint8_t *spi_data = (uint8_t *) pmt_u8vector_writeable_elements(data, data_len);

      // Make the USB packet
      if(!pkt->cs_spi_write(enables, format, opt, spi_data, data_len))
      {
        d_cs_usrp->send(s_cmd_usrp_write, 
                        pmt_list3(invocation_handle, 
                                  pmt_from_long(channel), 
                                  v_packet));
        
        goto new_packet;
      }
      
      if(verbose)
        std::cout << "[USRP_SERVER] Received SPI write\n";
    }
    
    //--------- SPI READ -----------//
    if(pmt_eq(subp_cmd, s_op_spi_read)) {
      
      long urid     = pmt_to_long(pmt_nth(0, subp_data));
      long enables  = pmt_to_long(pmt_nth(1, subp_data));
      long format   = pmt_to_long(pmt_nth(2, subp_data));
      long opt      = pmt_to_long(pmt_nth(3, subp_data));
      long n_bytes  = pmt_to_long(pmt_nth(4, subp_data));
      
      long srid;
      if((srid = next_rid()) == -1)
        goto subpkt_bail;

      d_rids[srid].owner = port->port_symbol();
      d_rids[srid].user_rid = urid;

      // Make the USB packet
      if(!pkt->cs_spi_read(srid, enables, format, opt, n_bytes))
      {
        d_cs_usrp->send(s_cmd_usrp_write, 
                        pmt_list3(invocation_handle, 
                                  pmt_from_long(channel), 
                                  v_packet));
        
        // Return the rid
        d_rids[srid].owner = PMT_NIL;

        goto new_packet;
      }
      
      if(verbose)
        std::cout << "[USRP_SERVER] Received SPI read\n";
    }

  subpkt_bail:
    curr_subpkt++;

  }


  // If the current packets length is > 0, we know there are subpackets that
  // need to be sent out still.
  if(pkt->payload_len() > 0)
    d_cs_usrp->send(s_cmd_usrp_write, 
                    pmt_list3(invocation_handle, 
                              pmt_from_long(channel), 
                              v_packet));

  return;
}

void
usrp_server::handle_cmd_start_recv_raw_samples(mb_port_sptr port, std::vector<struct channel_info> &chan_info, pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  long channel = pmt_to_long(pmt_nth(1, data));

  // Ensure the channel is valid and the caller owns the port
  if(!check_valid(port, channel, chan_info,
                  pmt_list2(s_response_xmit_raw_frame, invocation_handle)))
    return;

  // Already started receiving samples? (another start before a stop)
  // Check the RX channel bitmask.
  if(d_rx_chan_mask & (1 << channel)) {
    port->send(s_response_recv_raw_samples,
               pmt_list5(invocation_handle,
                         s_err_already_receiving,
                         PMT_NIL,
                         PMT_NIL,
                         PMT_NIL));
    return;
  }

  // We only need to generate a 'start reading' command down to the
  // low level interface if no other channel is already reading
  //
  // We carry this over the CS interface because the lower level
  // interface does not care about the channel, we only demux it
  // at the usrp_server on responses.
  if(d_rx_chan_mask == 0) {
    
    if(verbose)
      std::cout << "[USRP_SERVER] Sending read request down to start recv\n";

    d_cs_usrp->send(s_cmd_usrp_start_reading, pmt_list1(invocation_handle));
  }

  d_rx_chan_mask |= 1<<channel;
  
  return;
}

void
usrp_server::handle_cmd_stop_recv_raw_samples(
                        mb_port_sptr port, 
                        std::vector<struct channel_info> &chan_info, 
                        pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  long channel = pmt_to_long(pmt_nth(1, data));

  // FIX ME : we have no responses to send an error...
  // Ensure the channel is valid and the caller owns the port
  //if(!check_valid(port, channel, chan_info,
  //                pmt_list2(s_response_xmit_raw_frame, invocation_handle)))
  //  return;

  // Remove this hosts bit from the receiver mask
  d_rx_chan_mask &= ~(1<<channel);

  // We only need to generate a 'start reading' command down to the
  // low level interface if no other channel is already reading
  //
  // We carry this over the CS interface because the lower level
  // interface does not care about the channel, we only demux it
  // at the usrp_server on responses.
  if(d_rx_chan_mask == 0) {
    
    if(verbose)
      std::cout << "[USRP_SERVER] Sending stop reading request down\n";

    d_cs_usrp->send(s_cmd_usrp_stop_reading, pmt_list1(invocation_handle));
  }
  
  return;
}

// Read the packet header, determine the port by the channel owner
void
usrp_server::handle_response_usrp_read(pmt_t data)
{

  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);
  pmt_t v_pkt = pmt_nth(2, data);

  size_t n_bytes;
  size_t ignore;

  if (d_fake_rx) {

    pmt_t pkt = pmt_nth(2, data);

    d_rx[0]->send(s_response_recv_raw_samples,
                  pmt_list5(PMT_F,
                            PMT_T,
                            pkt,
                            pmt_from_long(0xffff),
                            PMT_NIL));

    return;
  }

  // Extract the packet and return appropriately
  transport_pkt *pkt = (transport_pkt *) pmt_u8vector_writeable_elements(v_pkt, n_bytes);

  // The channel is used to find the port to pass the samples on
  long channel = pkt->chan();
  long payload_len = pkt->payload_len();
  long port;

  // Ignore packets which seem to have incorrect size or size 0
  if(payload_len > pkt->max_payload() || payload_len == 0)
    return;
  
  // If the packet is a C/S packet, parse it separately
  if(channel == 0x1f) {
    parse_control_pkt(invocation_handle, pkt);
    return;
  }

  if((port = rx_port_index(d_chaninfo_rx[channel].owner)) == -1)
    return; // Don't know where to send the sample... possibility on abrupt close
    
  pmt_t v_samples = pmt_make_u8vector(payload_len, 0);
  uint8_t *samples = pmt_u8vector_writeable_elements(v_samples, ignore);
  
  memcpy(samples, pkt->payload(), payload_len);

  // Build a properties dictionary to store things such as the RSSI
  pmt_t properties =  pmt_make_dict();

  pmt_dict_set(properties,
               pmt_intern("rssi"),
               pmt_from_long(pkt->rssi()));

  if(pkt->overrun())
    pmt_dict_set(properties,
                 pmt_intern("overrun"),
                 PMT_T);

  if(pkt->underrun())
    pmt_dict_set(properties,
                 pmt_intern("underrun"),
                 PMT_T);

  d_rx[port]->send(s_response_recv_raw_samples,
                   pmt_list5(invocation_handle,
                             status,
                             v_samples,
                             pmt_from_long(pkt->timestamp()),
                             properties));
  return;
}

void
usrp_server::parse_control_pkt(pmt_t invocation_handle, transport_pkt *pkt)
{

  long payload_len = pkt->payload_len();
  long curr_payload = 0;
  long port;
  
  // We dispatch based on the control packet type, however we can extract the
  // opcode and the length immediately which is consistent in all responses.
  //
  // Since each control packet can have multiple responses, we keep reading the
  // lengths of each subpacket until we reach the payload length.  
  while(curr_payload < payload_len) {

    pmt_t sub_packet = pkt->read_subpacket(curr_payload);
    pmt_t op_symbol = pmt_nth(0, sub_packet);

    int len = pkt->cs_len(curr_payload);

    if(verbose)
      std::cout << "[USRP_SERVER] Parsing subpacket " 
                << op_symbol << " ... length " << len << std::endl;

    //----------------- PING RESPONSE ------------------//
    if(pmt_eq(op_symbol, s_op_ping_fixed_reply)) {

      long srid     = pmt_to_long(pmt_nth(1, sub_packet));
      pmt_t pingval = pmt_nth(2, sub_packet);

      long urid = d_rids[srid].user_rid;
      
      if(verbose)
        std::cout << "[USRP_SERVER] Found ping response "
                  << "("
                  << "URID: " << urid << ", "
                  << "SRID: " << srid << ", "
                  << "VAL: " << pingval 
                  << ")\n";
      
      // Do some bounds checking incase of bogus/corrupt responses
      if(srid > D_MAX_RID)
        return;

      pmt_t owner = d_rids[srid].owner;

      // FIXME: should be 1 response for all subpackets here ?
      if((port = tx_port_index(owner)) != -1)
        d_tx[port]->send(s_response_from_control_channel,
                         pmt_list4(invocation_handle,
                                   PMT_T,
                                   pmt_list2(s_op_ping_fixed_reply, // subp
                                             pmt_list2(pmt_from_long(urid), 
                                                       pingval)),
                                   pmt_from_long(pkt->timestamp())));
    }
    
    //----------------- READ REG RESPONSE ------------------//
    else if(pmt_eq(op_symbol, s_op_read_reg_reply)) {

      long srid     = pmt_to_long(pmt_nth(1, sub_packet));
      pmt_t reg_num = pmt_nth(2, sub_packet);
      pmt_t reg_val = pmt_nth(3, sub_packet);

      long urid = d_rids[srid].user_rid;
      
      if(verbose)
        std::cout << "[USRP_SERVER] Found read register response "
                  << "("
                  << "URID: " << urid << ", "
                  << "SRID: " << srid << ", "
                  << "REG: " << reg_num << ", "
                  << "VAL: " << reg_val 
                  << ")\n";

      // Do some bounds checking to avoid seg faults
      if(srid > D_MAX_RID)
        return;
      
      pmt_t owner = d_rids[srid].owner;

      // FIXME: should be 1 response for all subpackets here ?
      if((port = tx_port_index(owner)) != -1)
        d_tx[port]->send(s_response_from_control_channel,
                         pmt_list4(invocation_handle,
                                   PMT_T,
                                   pmt_list2(s_op_read_reg_reply, // subp
                                             pmt_list3(pmt_from_long(urid), 
                                                       reg_num, 
                                                       reg_val)),
                                   pmt_from_long(pkt->timestamp())));
    }

    //------------------ I2C READ REPLY -------------------//
    else if(pmt_eq(op_symbol, s_op_i2c_read_reply)) {

      long srid       = pmt_to_long(pmt_nth(1, sub_packet));
      pmt_t i2c_addr  = pmt_nth(2, sub_packet);
      pmt_t i2c_data  = pmt_nth(3, sub_packet);

      long urid = d_rids[srid].user_rid;

      if(verbose)
        std::cout << "[USRP_SERVER] Found i2c read reply "
                  << "("
                  << "URID: " << urid << ", "
                  << "SRID: " << srid << ", "
                  << "Addr: " << i2c_addr << ", "
                  << "Data: " << i2c_data
                  << ")\n";
      
      // Do some bounds checking to avoid seg faults
      if(srid > D_MAX_RID)
        return;

      pmt_t owner = d_rids[srid].owner;

      if((port = tx_port_index(owner)) != -1)
        d_tx[port]->send(s_response_from_control_channel,
                         pmt_list4(invocation_handle,
                                   PMT_T,
                                   pmt_list2(s_op_i2c_read_reply,
                                             pmt_list3(pmt_from_long(urid), 
                                                       i2c_addr,
                                                       i2c_data)),
                                   pmt_from_long(pkt->timestamp())));
    }

    //------------------ SPI READ REPLY -------------------//
    else if(pmt_eq(op_symbol, s_op_spi_read_reply)) {
      
      long srid       = pmt_to_long(pmt_nth(1, sub_packet));
      pmt_t spi_data  = pmt_nth(2, sub_packet);
      
      long urid = d_rids[srid].user_rid;

      if(verbose)
        std::cout << "[USRP_SERVER] Found SPI read reply "
                  << "("
                  << "URID: " << urid << ", "
                  << "SRID: " << srid << ", "
                  << "Data: " << spi_data
                  << ")\n";

      // Bounds check the RID
      if(srid > D_MAX_RID)
        return;

      pmt_t owner = d_rids[srid].owner;

      if((port = tx_port_index(owner)) != -1)
        d_tx[port]->send(s_response_from_control_channel,
                         pmt_list4(invocation_handle,
                                   PMT_T,
                                   pmt_list2(s_op_spi_read_reply,
                                             pmt_list2(pmt_from_long(urid), 
                                                       spi_data)),
                                   pmt_from_long(pkt->timestamp())));
    }

    // Each subpacket has an unaccounted for 2 bytes which is the opcode
    // and the length field
    curr_payload += len + 2;
    
    // All subpackets are 32-bit aligned
    int align_offset = 4 - (curr_payload % 4);

    if(align_offset != 4)
      curr_payload += align_offset;
  }
}

void
usrp_server::recall_defer_queue()
{

  std::vector<mb_message_sptr> recall;

  while(!d_defer_queue.empty()) {
    recall.push_back(d_defer_queue.front());
    d_defer_queue.pop();
  }

  // Parse the messages that were queued while waiting for an open response
  for(int i=0; i < (int)recall.size(); i++) 
    handle_message(recall[i]);

  return;
}

bool
usrp_server::check_valid(mb_port_sptr port,
                         long channel,
                         std::vector<struct channel_info> &chan_info,
                         pmt_t signal_info)
{

  pmt_t response_signal = pmt_nth(0, signal_info);
  pmt_t invocation_handle = pmt_nth(1, signal_info);

  // not a valid channel number?
  if(channel >= (long)chan_info.size() && channel != 0x1f) {
    port->send(response_signal, 
               pmt_list2(invocation_handle, 
                         s_err_channel_invalid));

    if(verbose)
      std::cout << "[USRP_SERVER] Invalid channel number for event " 
                << response_signal << std::endl;
    return false;
  }
  
  // not the owner of the port?
  if(chan_info[channel].owner != port->port_symbol()) {
    port->send(response_signal, 
               pmt_list2(invocation_handle, 
                         s_err_channel_permission_denied));
    
    if(verbose)
      std::cout << "[USRP_SERVER] Invalid permissions"
                << " for " << response_signal
                << " from " << port->port_symbol()
                << " proper owner is " << chan_info[channel].owner
                << " on channel " << channel
                << " invocation " << invocation_handle
                << std::endl;
    return false;
  }

  return true;
}

// Goes through the vector of RIDs and retreieves an
// available one for use
long
usrp_server::next_rid()
{
  for(int i = 0; i < D_MAX_RID; i++)
    if(pmt_eqv(d_rids[i].owner, PMT_NIL))
      return i;

  return -1;
}

REGISTER_MBLOCK_CLASS(usrp_server);
