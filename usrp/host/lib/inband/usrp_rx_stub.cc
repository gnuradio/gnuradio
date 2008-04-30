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

#include <usrp_rx_stub.h>

#include <iostream>
#include <vector>
#include <usb.h>
#include <mb_class_registry.h>
#include <usrp_inband_usb_packet.h>
#include <fpga_regs_common.h>
#include "usrp_standard.h"
#include <stdio.h>
#include <string.h>
#include <ui_nco.h>
#include <fstream>

#include <symbols_usrp_rx_cs.h>

typedef usrp_inband_usb_packet transport_pkt;

static const bool verbose = false;

bool usrp_rx_stop;

// Used for the fake control packet response code to send the responses back up
// the RX.  The TX stub dumps responses in to this queue.
std::queue<pmt_t> d_cs_queue;

usrp_rx_stub::usrp_rx_stub(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(rt, instance_name, user_arg),
    d_samples_per_frame((long)(126)),
    d_amplitude(16384),
    d_disk_write(false)
{
  d_cs = define_port("cs", "usrp-rx-cs", true, mb_port::EXTERNAL);
  
  // initialize NCO
  double freq = 100e3;
  int interp = 32;			    // 32 -> 4MS/s
  double sample_rate = 128e6 / interp;	
  d_nco.set_freq(2*M_PI * freq/sample_rate);

  //d_disk_write = true;
  
  if(d_disk_write)
    d_ofile.open("raw_rx.dat",std::ios::binary|std::ios::out);
  
  usrp_rx_stop = false;
}

usrp_rx_stub::~usrp_rx_stub() 
{
  if(d_disk_write)
    d_ofile.close();
}

void 
usrp_rx_stub::initial_transition()
{
  
}

void
usrp_rx_stub::handle_message(mb_message_sptr msg)
{
  pmt_t event = msg->signal();
  pmt_t port_id = msg->port_id();
  pmt_t data = msg->data(); 

  // Theoretically only have 1 message to ever expect, but
  // want to make sure its at least what we want
  if(pmt_eq(port_id, d_cs->port_symbol())) {

    if(verbose)
      std::cout << "[USRP_RX_STUB] Starting...\n";
    
    if(pmt_eqv(event, s_cmd_usrp_rx_start_reading))
      read_and_respond(data);
  }
}

void
usrp_rx_stub::read_and_respond(pmt_t data)
{

  while(!usrp_rx_stop) {

    long nsamples_this_frame = d_samples_per_frame;

    size_t nshorts = 2 * nsamples_this_frame;	// 16-bit I & Q
    long channel = 0;
    long n_bytes = nshorts*2;
    pmt_t uvec = pmt_make_s16vector(nshorts, 0);
    size_t ignore;
    int16_t *samples = pmt_s16vector_writeable_elements(uvec, ignore);

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
    
    if(d_disk_write)
      d_ofile.write((const char *)samples, n_bytes);
  
    pmt_t v_pkt = pmt_make_u8vector(sizeof(transport_pkt), 0);
    transport_pkt *pkt =
      (transport_pkt *) pmt_u8vector_writeable_elements(v_pkt, ignore);

    pkt->set_header(0, channel, 0, n_bytes);
    pkt->set_timestamp(0xffffffff);
    memcpy(pkt->payload(), samples, n_bytes);
    
    d_cs->send(s_response_usrp_rx_read, pmt_list3(PMT_NIL, PMT_T, v_pkt));

    // Now lets check the shared CS queue between the TX and RX stub.  Each
    // element in a queue is a list where the first element is an invocation
    // handle and the second element is a PMT u8 vect representation of the
    // CS packet response which can just be passed transparently.
    while(!d_cs_queue.empty()) {
      
      pmt_t cs_pkt = d_cs_queue.front();
      d_cs_queue.pop();

      pmt_t invocation_handle = pmt_nth(0, cs_pkt);
      pmt_t v_pkt = pmt_nth(1, cs_pkt);

      d_cs->send(s_response_usrp_rx_read,   
                 pmt_list3(invocation_handle, 
                           PMT_T, 
                           v_pkt));  // Take the front CS pkt

      
      if(verbose)
        std::cout << "[USRP_RX_STUB] Received CS response from TX stub\n";
    }

  }
  
  usrp_rx_stop = false;

  if(verbose)
    std::cout << "[USRP_RX_STUB] Got fake RX stop\n";

}

REGISTER_MBLOCK_CLASS(usrp_rx_stub);
