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

#include <usrp_tx.h>
#include <iostream>
#include <usb.h>
#include <mblock/class_registry.h>
#include <usrp_inband_usb_packet.h>
#include <fpga_regs_common.h>
#include <usrp_standard.h>
#include <stdio.h>

#include <symbols_usrp_tx_cs.h>

typedef usrp_inband_usb_packet transport_pkt;

static const bool verbose = false;

usrp_tx::usrp_tx(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(rt, instance_name, user_arg),
    d_disk_write(false)
{
  d_cs = define_port("cs", "usrp-tx-cs", true, mb_port::EXTERNAL);
  
  //d_disk_write=true;
  
  if(d_disk_write) {
    d_ofile.open("tx_data.dat",std::ios::binary|std::ios::out);
    d_cs_ofile.open("tx_cs.dat",std::ios::binary|std::ios::out);
  }
}

usrp_tx::~usrp_tx() 
{
  if(d_disk_write) {
    d_ofile.close();
    d_cs_ofile.close();
  }
}

void 
usrp_tx::initial_transition()
{
  
}

/*!
 * \brief Handles incoming signals to to the m-block, wihch should only ever be
 * a single message: cmd-usrp-tx-write.
 */
void
usrp_tx::handle_message(mb_message_sptr msg)
{
  pmt_t event = msg->signal();
  pmt_t port_id = msg->port_id();
  pmt_t data = msg->data(); 

  // Theoretically only have 1 message to ever expect, but
  // want to make sure its at least what we want
  if(pmt_eq(port_id, d_cs->port_symbol())) {
    
    if(pmt_eqv(event, s_cmd_usrp_tx_write))
      write(data);
  }
}

/*!
 * \brief Performs the actual writing of data to the USB bus, called by
 * handle_message() when a cmd-usrp-tx-write signal is received.
 *
 * The \p data parameter is a PMT list which contains three mandatory elements,
 * in the following order: an invocation handle, a channel, and a uniform vector
 * of memory which contains the packets to be written to the bus.
 */
void
usrp_tx::write(pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t channel = pmt_nth(1, data);
  pmt_t v_packets = pmt_nth(2, data);
  d_utx = boost::any_cast<usrp_standard_tx_sptr>(pmt_any_ref(pmt_nth(3, data)));

  size_t n_bytes;
  bool underrun;  // this will need to go, as it is taken care of in the packet headers

  transport_pkt *pkts = (transport_pkt *) pmt_u8vector_writable_elements(v_packets, n_bytes);

  int ret = d_utx->write (pkts, n_bytes, &underrun);
  
  if (0 && underrun)
    fprintf(stderr, "uU");

  if (ret == (int) n_bytes) {
    if (verbose)
      std::cout << "[usrp_server] Write of " << n_bytes << " successful\n";
    // need to respond with the channel so the USRP server knows who to forward the result of
    // the write to by looking up the owner of the channel
    d_cs->send(s_response_usrp_tx_write,
	       pmt_list3(invocation_handle, PMT_T, channel));
  }
  else {
    if (verbose)
      std::cout << "[usrp_server] Error writing " << n_bytes << " bytes to USB bus\n";
    d_cs->send(s_response_usrp_tx_write,
	       pmt_list3(invocation_handle, PMT_F, channel));
  }
    
  long n_packets = 
    static_cast<long>(std::ceil(n_bytes / (double)transport_pkt::max_pkt_size()));

  for(int i=0; i < n_packets; i++) {
    
    if(d_disk_write) {
      if(pkts[i].chan() == CONTROL_CHAN)
        d_cs_ofile.write((const char *)&pkts[i], transport_pkt::max_pkt_size());
      else
        d_ofile.write((const char *)&pkts[i], transport_pkt::max_pkt_size());

      d_cs_ofile.flush();
      d_ofile.flush();
    }
  }


  return;
}

REGISTER_MBLOCK_CLASS(usrp_tx);
