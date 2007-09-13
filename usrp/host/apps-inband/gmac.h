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

#ifndef INCLUDED_GMAC_H
#define INCLUDED_GMAC_H

#include <mb_mblock.h>

class gmac;

class gmac : public mb_mblock
{

  // The state is used to determine how to handle incoming messages and of
  // course, the state of the MAC protocol.
  enum state_t {
    INIT,
    OPENING_USRP,
    ALLOCATING_CHANNELS,
    INIT_GMAC,
    IDLE,
    CLOSING_CHANNELS,
    CLOSING_USRP,
  };
  state_t	d_state;

  // Ports used for applications to connect to this block
  mb_port_sptr		  d_tx, d_rx, d_cs;

  // Ports to connect to usrp_server (us)
  mb_port_sptr      d_us_tx, d_us_rx, d_us_cs;

  // The channel numbers assigned for use
  pmt_t d_us_rx_chan, d_us_tx_chan;

  pmt_t d_which_usrp;

  bool d_carrier_sense;
  long d_cs_thresh;
  long d_cs_deadline;

  enum FPGA_REGISTERS {
    REG_CS_THRESH = 1,
    REG_CS_DEADLINE = 2
  };
  
 public:
  gmac(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg);
  ~gmac();
  void handle_message(mb_message_sptr msg);

 private:
  void define_ports();
  void initialize_usrp();
  void initialize_gmac();
  void set_carrier_sense(bool toggle, long threshold, long deadline, pmt_t invocation);
  void allocate_channels();
  void enter_receiving();
  void enter_idle();
  void close_channels();
  void open_usrp();
  void close_usrp();
  void handle_cmd_tx_pkt(pmt_t data);
  void handle_response_xmit_raw_frame(pmt_t data);
  bool carrier_sense_pkt(pmt_t pkt_properties);
  void handle_cmd_carrier_sense_enable(pmt_t data);
  void handle_cmd_carrier_sense_threshold(pmt_t data);
  void handle_cmd_carrier_sense_disable(pmt_t data);
  void handle_cmd_carrier_sense_deadline(pmt_t data);
 
};

#endif // INCLUDED_GMAC_H
