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

#include <iostream>
#include <vector>
#include <usb.h>
#include <mb_class_registry.h>
#include <usrp_tx_stub.h>
#include <usrp_inband_usb_packet.h>
#include <fpga_regs_common.h>
#include "usrp_standard.h"
#include <stdio.h>
#include <fstream>
#include <usrp_rx_stub.h>

#include <symbols_usrp_tx_cs.h>

typedef usrp_inband_usb_packet transport_pkt;

static const bool verbose = false;

usrp_tx_stub::usrp_tx_stub(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(rt, instance_name, user_arg),
    d_disk_write(false)
{
  d_cs = define_port("cs", "usrp-tx-cs", true, mb_port::EXTERNAL);
  
  //d_disk_write=true;
  
  if(d_disk_write) {
    d_ofile.open("tx_stub_data.dat",std::ios::binary|std::ios::out);
    d_cs_ofile.open("tx_stub_cs.dat",std::ios::binary|std::ios::out);
  }
}

usrp_tx_stub::~usrp_tx_stub() 
{
  if(d_disk_write) {
    d_ofile.close();
    d_cs_ofile.close();
  }
}

void 
usrp_tx_stub::initial_transition()
{
  
}

void
usrp_tx_stub::handle_message(mb_message_sptr msg)
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

void
usrp_tx_stub::write(pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t channel = pmt_nth(1, data);
  pmt_t v_packets = pmt_nth(2, data);
  d_utx = boost::any_cast<usrp_standard_tx *>(pmt_any_ref(pmt_nth(3, data)));

  size_t n_bytes;

  transport_pkt *pkts = (transport_pkt *) pmt_u8vector_writeable_elements(v_packets, n_bytes);
  long n_packets = static_cast<long>(std::ceil(n_bytes / (double)transport_pkt::max_pkt_size()));
  
  // Parse the packets looking for C/S packets and dump them to a disk if
  // necessary
  for(long i=0; i<n_packets; i++) {

    if(d_disk_write) {
      if(pkts[i].chan() == CONTROL_CHAN)
        d_cs_ofile.write((const char *)&pkts[i], transport_pkt::max_pkt_size());
      else
        d_ofile.write((const char *)&pkts[i], transport_pkt::max_pkt_size());

      d_cs_ofile.flush();
      d_ofile.flush();
    }

    if(pkts[i].chan() == CONTROL_CHAN)
      parse_cs(invocation_handle, pkts[i]);
  }

  d_cs->send(s_response_usrp_tx_write,
       pmt_list3(invocation_handle, PMT_T, channel));

  return;
}

void
usrp_tx_stub::parse_cs(pmt_t invocation_handle, transport_pkt pkt)
{
  
  long payload_len = pkt.payload_len();
  long curr_payload = 0;
      
  size_t ignore;

  // There is the possibility that the responses for a single USB packet full of
  // CS packets will not fit back in a single USB packet, considering some
  // responses are greater than their commands (read registers).
 new_packet:
  pmt_t v_pkt = pmt_make_u8vector(sizeof(transport_pkt), 0);
  
  transport_pkt *q_pkt =
    (transport_pkt *) pmt_u8vector_writeable_elements(v_pkt, ignore);
      
  q_pkt->set_header(0, CONTROL_CHAN, 0, 0);
  q_pkt->set_timestamp(0xffffffff);

  // We dispatch based on the control packet type, however we can extract the
  // opcode and the length immediately which is consistent in all responses.
  //
  // Since each control packet can have multiple responses, we keep reading the
  // lengths of each subpacket until we reach the payload length.  
  while(curr_payload < payload_len) {

    pmt_t sub_packet = pkt.read_subpacket(curr_payload);
    pmt_t op_symbol = pmt_nth(0, sub_packet);

    int len = pkt.cs_len(curr_payload);
      
    if(verbose)
      std::cout << "[USRP_TX_STUB] Parsing subpacket " 
                << op_symbol << " ... length " << len << std::endl;

    //----------------- PING FIXED ------------------//
    if(pmt_eq(op_symbol, s_op_ping_fixed)) {

      long rid = pmt_to_long(pmt_nth(1, sub_packet));
      long pingval = pmt_to_long(pmt_nth(2, sub_packet));

      // Generate a reply and put it in the queue for the RX stub to read
      if(!q_pkt->cs_ping_reply(rid, pingval))
        goto new_packet;

      if(verbose)
        std::cout << "[USRP_TX_STUB] Generated ping response "
                  << "("
                  << "RID: " << rid << ", "
                  << "VAL: " << pingval 
                  << ")\n";
    }

    //----------------- READ REG ------------------//
    if(pmt_eq(op_symbol, s_op_read_reg)) {

      long rid = pmt_to_long(pmt_nth(1, sub_packet));
      long reg_num = pmt_to_long(pmt_nth(2, sub_packet));
      long reg_val = 0xdeef;

      // Generate a reply and put it in the queue for the RX stub to read
      if(!q_pkt->cs_read_reg_reply(rid, reg_num, reg_val))
        goto new_packet;

      if(verbose)
        std::cout << "[USRP_TX_STUB] Generated read register response "
                  << "("
                  << "RID: " << rid << ", "
                  << "REG: " << reg_num << ", "
                  << "VAL: " << reg_val
                  << ")\n";
    }
    
    //----------------- DELAY ------------------//
    if(pmt_eq(op_symbol, s_op_delay)) {

      long ticks = pmt_to_long(pmt_nth(1, sub_packet));

      if(verbose)
        std::cout << "[USRP_TX_STUB] Received delay command "
                  << "("
                  << "Ticks: " << ticks
                  << ")\n";
    }

    //----------------- WRITE REG ------------------//
    if(pmt_eq(op_symbol, s_op_write_reg)) {

      pmt_t reg_num = pmt_nth(1, sub_packet);
      pmt_t reg_val = pmt_nth(2, sub_packet);

      if(verbose)
        std::cout << "[USRP_TX_STUB] Received write register command "
                  << "("
                  << "RegNum: " << reg_num << ", "
                  << "Val: " << reg_val
                  << ")\n";
    }
    
    //----------------- WRITE REG MASK ---------------//
    if(pmt_eq(op_symbol, s_op_write_reg_masked)) {

      pmt_t reg_num = pmt_nth(1, sub_packet);
      pmt_t reg_val = pmt_nth(2, sub_packet);
      pmt_t mask    = pmt_nth(3, sub_packet);

      if(verbose)
        std::cout << "[USRP_TX_STUB] Received write register command "
                  << "("
                  << "RegNum: " << reg_num << ", "
                  << "Val: " << reg_val << ", "
                  << "Mask: " << mask
                  << ")\n";
    }

    //---------------- I2C WRITE ------------------//
    if(pmt_eq(op_symbol, s_op_i2c_write)) {
      pmt_t i2c_addr  = pmt_nth(1, sub_packet);
      pmt_t i2c_data  = pmt_nth(2, sub_packet);

      if(verbose)
        std::cout << "[USRP_TX_STUB] Received i2c write command "
                  << "("
                  << "Addr: " << i2c_addr << ", "
                  << "Data: " << i2c_data
                  << ")\n";
    }

    //---------------- I2C READ ------------------//
    if(pmt_eq(op_symbol, s_op_i2c_read)) {
      long rid       = pmt_to_long(pmt_nth(1, sub_packet));
      long i2c_addr  = pmt_to_long(pmt_nth(2, sub_packet));
      long i2c_bytes = pmt_to_long(pmt_nth(3, sub_packet));

      // Create data to place as a response, filled with 0xff
      size_t ignore;
      pmt_t i2c_data = pmt_make_u8vector(i2c_bytes, 0xff);
      uint8_t *w_data = (uint8_t *) pmt_u8vector_writeable_elements(i2c_data, ignore);

      // Generate a reply and put it in the queue for the RX stub to read
      if(!q_pkt->cs_i2c_read_reply(rid, i2c_addr, w_data, i2c_bytes))
        goto new_packet;

      if(verbose)
        std::cout << "[USRP_TX_STUB] Received i2c read "
                  << "("
                  << "RID: " << rid << ", "
                  << "Addr: " << i2c_addr << ", "
                  << "Bytes: " << i2c_bytes
                  << ")\n";
    }
    
    //---------------- SPI WRITE ------------------//
    if(pmt_eq(op_symbol, s_op_spi_write)) {
      long enables  = pmt_to_long(pmt_nth(1, sub_packet));
      long format   = pmt_to_long(pmt_nth(2, sub_packet));
      long opt      = pmt_to_long(pmt_nth(3, sub_packet));
      pmt_t data    = pmt_nth(4, sub_packet);

      if(verbose)
        std::cout << "[USRP_TX_STUB] Received spi write command "
                  << "("
                  << "Enables: " << enables << ", "
                  << "Format: " << format << ", "
                  << "Options: " << opt << ", "
                  << "Data: " << data
                  << ")\n";
    }

    //---------------- SPI READ ------------------//
    if(pmt_eq(op_symbol, s_op_spi_read)) {
      long rid      = pmt_to_long(pmt_nth(1, sub_packet));
      long enables  = pmt_to_long(pmt_nth(2, sub_packet));
      long format   = pmt_to_long(pmt_nth(3, sub_packet));
      long opt      = pmt_to_long(pmt_nth(4, sub_packet));
      long n_bytes  = pmt_to_long(pmt_nth(5, sub_packet));

      // Create data to place as a fake response
      size_t ignore;
      pmt_t spi_data = pmt_make_u8vector(n_bytes, 0xff);
      uint8_t *w_data = (uint8_t *) pmt_u8vector_writeable_elements(spi_data, ignore);

      // Generate a reply and put it in the queue for the RX stub to read
      if(!q_pkt->cs_spi_read_reply(rid, w_data, n_bytes))
        goto new_packet;

      if(verbose)
        std::cout << "[USRP_TX_STUB] Received spi read command "
                  << "("
                  << "RID: " << rid << ", "
                  << "Enables: " << enables << ", "
                  << "Format: " << format << ", "
                  << "Options: " << opt << ", "
                  << "Bytes: " << n_bytes
                  << ")\n";
      
    }

    // Each subpacket has an unaccounted for 2 bytes which is the opcode
    // and the length field
    curr_payload += len + 2;

    // All subpackets are 32-bit aligned
    int align_offset = 4 - (curr_payload % 4);

    if(align_offset != 4)
      curr_payload += align_offset;

  }

  // If the packet has data in the payload, it needs queued
  if(q_pkt->payload_len() > 0)
    d_cs_queue.push(pmt_list2(invocation_handle, v_pkt));

  return;
}

REGISTER_MBLOCK_CLASS(usrp_tx_stub);
