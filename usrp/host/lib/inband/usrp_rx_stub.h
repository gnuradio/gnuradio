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
#ifndef INCLUDED_USRP_RX_STUB_H
#define INCLUDED_USRP_RX_STUB_H

#include <mb_mblock.h>
#include <vector>
#include "usrp_standard.h"
#include "ui_nco.h"
#include <fstream>
#include <queue>
#include <usrp_inband_usb_packet.h>

typedef usrp_inband_usb_packet transport_pkt;

extern bool usrp_rx_stop;   // used to communicate a 'stop' to the RX stub
extern std::queue<pmt_t> d_cs_queue;

/*!
 * \brief Implements the low level usb interface to the USRP
 */
class usrp_rx_stub : public mb_mblock
{
 public:

  mb_port_sptr	d_cs;
  usrp_standard_rx* d_urx;
  
  long		d_samples_per_frame;
  
  // for generating sine wave output
  ui_nco<float,float>	d_nco;
  double	        d_amplitude;

  bool d_disk_write;

  std::ofstream d_ofile;
  
 public:
  usrp_rx_stub(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg);
  ~usrp_rx_stub();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 private:
  void read_and_respond(pmt_t data);
  void read_data();
 
};
  

#endif /* INCLUDED_USRP_RX_H */

