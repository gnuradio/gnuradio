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
#ifndef INCLUDED_USRP_TX_H
#define INCLUDED_USRP_TX_H

#include <mblock/mblock.h>
#include <fstream>

class usrp_standard_tx;

/*!
 * \brief Implements the low level usb interface to the USRP
 */
class usrp_tx : public mb_mblock
{
  mb_port_sptr		d_cs;
  usrp_standard_tx     *d_utx;
  
  bool d_disk_write;
  std::ofstream d_ofile;
  std::ofstream d_cs_ofile;
  
 public:
  usrp_tx(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg);
  ~usrp_tx();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 private:
  void write(pmt_t data);
};
  

#endif /* INCLUDED_USRP_TX_H */

