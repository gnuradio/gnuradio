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

#include <usrp_rx.h>

#include <usrp_standard.h>
#include <iostream>
#include <vector>
#include <usb.h>
#include <mblock/class_registry.h>
#include <usrp_inband_usb_packet.h>
#include <fpga_regs_common.h>
#include <stdio.h>

#include <symbols_usrp_rx_cs.h>

typedef usrp_inband_usb_packet transport_pkt;

static const bool verbose = false;

bool usrp_rx_stop;

usrp_rx::usrp_rx(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(rt, instance_name, user_arg),
    d_disk_write(false),
    d_disk_write_pkt(false)   // if true, writes full packet, else just the payload
{
  d_cs = define_port("cs", "usrp-rx-cs", true, mb_port::EXTERNAL);
  
  if(d_disk_write) {
    d_ofile0.open("rx_data_chan0.dat",std::ios::binary|std::ios::out);
    d_ofile1.open("rx_data_chan1.dat",std::ios::binary|std::ios::out);
    d_cs_ofile.open("rx_cs.dat",std::ios::binary|std::ios::out);
  }
  
  usrp_rx_stop = false;

}

usrp_rx::~usrp_rx() 
{
  if(d_disk_write) {
    d_ofile0.close();
    d_ofile1.close();
    d_cs_ofile.close();
  }
}

void 
usrp_rx::initial_transition()
{
  
}

/*!
 * \brief Handles incoming signals to to the m-block, wihch should only ever be
 * a single message: cmd-usrrp-rx-start-reading.  There is no signal to stop
 * reading as the m-block goes in to a forever loop to read inband packets from
 * the bus.
 */
void
usrp_rx::handle_message(mb_message_sptr msg)
{
  pmt_t event = msg->signal();
  pmt_t port_id = msg->port_id();
  pmt_t data = msg->data(); 

  // Theoretically only have 1 message to ever expect, but
  // want to make sure its at least what we want
  if(pmt_eq(port_id, d_cs->port_symbol())) {
    
    if(pmt_eqv(event, s_cmd_usrp_rx_start_reading))
      read_and_respond(data);
  }
}

/*!
 * \brief Performs the actual reading of data from the USB bus, called by
 * handle_message() when a cmd-usrp-rx-start-reading signal is received.  
 *
 * The method enters a forever loop where it continues to read data from the bus
 * and generate read responses to the higher layer.  Currently, shared memory is
 * used to exit this loop.
 *
 * The \p data parameter is a PMT list which contains only a single element, an
 * invocation handle which will be returned with all read respones.
 */
void
usrp_rx::read_and_respond(pmt_t data)
{
  size_t ignore;
  bool underrun;
  unsigned int n_read;
  unsigned int pkt_size = sizeof(transport_pkt);

  pmt_t invocation_handle = pmt_nth(0, data);

  // Need the handle to the RX port to send responses, this is passed
  // by the USRP interface m-block
  pmt_t handle = pmt_nth(1, data);
  d_urx = 
    boost::any_cast<usrp_standard_rx_sptr>(pmt_any_ref(handle));

  if(verbose)
    std::cout << "[usrp_rx] Waiting for packets..\n";

  // Read by 512 which is packet size and send them back up
  while(!usrp_rx_stop) {

    pmt_t v_pkt = pmt_make_u8vector(pkt_size, 0);
    transport_pkt *pkt = 
      (transport_pkt *) pmt_u8vector_writable_elements(v_pkt, ignore);

    n_read = d_urx->read(pkt, pkt_size, &underrun);

    if(n_read != pkt_size) {
      std::cerr << "[usrp_rx] Error reading packet, shutting down\n";
      d_cs->send(s_response_usrp_rx_read, 
                 pmt_list3(PMT_NIL, PMT_F, PMT_NIL));
      return;
    }

    if(underrun && verbose && 0)
      std::cout << "[usrp_rx] Underrun\n";

    d_cs->send(s_response_usrp_rx_read, 
               pmt_list3(PMT_NIL, PMT_T, v_pkt));
    if(verbose && 0)
      std::cout << "[usrp_rx] Read 1 packet\n";
    
    if(d_disk_write) {
      if(pkt->chan() == CONTROL_CHAN)
        d_cs_ofile.write((const char *)pkt, transport_pkt::max_pkt_size());
      else {
        if(d_disk_write_pkt) {
          if(pkt->chan() == 0)
            d_ofile0.write((const char *)pkt, transport_pkt::max_pkt_size());
          else if(pkt->chan() == 1)
            d_ofile1.write((const char *)pkt, transport_pkt::max_pkt_size());
        } else {
          if(pkt->chan() == 0)
            d_ofile0.write((const char *)pkt->payload(), transport_pkt::max_payload());
          else if(pkt->chan() == 1)
            d_ofile1.write((const char *)pkt->payload(), transport_pkt::max_payload());
        }
      }

      d_cs_ofile.flush();
      d_ofile0.flush();
      d_ofile1.flush();
    }
  }
  
  usrp_rx_stop = false;

  if(verbose) {
    std::cout << "[USRP_RX] Stopping...\n";
    fflush(stdout);
  }
}

REGISTER_MBLOCK_CLASS(usrp_rx);
