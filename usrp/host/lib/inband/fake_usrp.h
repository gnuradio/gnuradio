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
#ifndef INCLUDED_FAKE_USRP_H
#define INCLUDED_FAKE_USRP_H

#include <usrp_inband_usb_packet.h>
typedef usrp_inband_usb_packet transport_pkt;

/*!
 * \brief Implements a fake USRP for testing without hardware
 */
class fake_usrp
{
 public:
  fake_usrp();
  ~fake_usrp();
  long write_bus(transport_pkt *pkts, long n_bytes);

 protected:
  long data_block(transport_pkt *pkts, long n_bytes);
  long control_block(transport_pkt *pkts, long n_bytes);
};

#endif /* INCLUDED_FAKE_USRP_H */

