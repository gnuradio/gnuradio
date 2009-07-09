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
#ifndef INCLUDED_USRP_TX_STUB_H
#define INCLUDED_USRP_TX_STUB_H

#include <mblock/mblock.h>
#include <vector>
#include "usrp_standard.h"
#include <fstream>
#include <usrp_inband_usb_packet.h>

typedef usrp_inband_usb_packet transport_pkt;

/*!
 * \brief Implements the low level usb interface to the USRP
 */
class usrp_tx_stub : public mb_mblock
{
 public:

  mb_port_sptr	d_cs;
  usrp_standard_tx* d_utx;
  
  std::ofstream d_ofile;
  std::ofstream d_cs_ofile;

  bool d_disk_write;

 public:
  usrp_tx_stub(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg);
  ~usrp_tx_stub();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 private:
  void write(pmt_t data);
  void parse_cs(pmt_t invocation_handle, transport_pkt pkt);
 
};
  

#endif /* INCLUDED_USRP_TX_STUB_H */

