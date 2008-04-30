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
#ifndef INCLUDED_USRP_USB_INTERFACE_H
#define INCLUDED_USRP_USB_INTERFACE_H

#include <mb_mblock.h>
#include <vector>
#include "usrp_standard.h"

/*!
 * \brief Implements the low level usb interface to the USRP
 */
class usrp_usb_interface : public mb_mblock
{
 public:

  usrp_standard_tx* d_utx;
  usrp_standard_rx* d_urx;
  
  mb_port_sptr	d_cs;
  mb_port_sptr  d_rx_cs;
  mb_port_sptr  d_tx_cs;
  
  long d_ntx_chan;
  long d_nrx_chan;

  bool d_fake_usrp;

  bool d_rx_reading;

  long d_interp_tx;
  long d_decim_rx;

  long d_rf_freq;

  std::string d_rbf;

 public:
  usrp_usb_interface(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg);
  ~usrp_usb_interface();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 private:
  void handle_cmd_open(pmt_t data);
  void handle_cmd_close(pmt_t data);
  void handle_cmd_write(pmt_t data);
  void handle_cmd_start_reading(pmt_t data);
  void handle_cmd_stop_reading(pmt_t data);
 
};
  

#endif /* INCLUDED_USRP_USB_INTERFACE_H */
