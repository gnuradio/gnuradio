/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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
#ifndef INCLUDED_USRP_SERVER_H
#define INCLUDED_USRP_SERVER_H

#include <mb_mblock.h>
#include <vector>

/*!
 * \brief Implements the lowest-level mblock interface to the USRP
 */
class usrp_server : public mb_mblock
{
public:

  enum error_codes {
    RQSTD_CAPACITY_UNAVAIL = 0,
    CHANNEL_UNAVAIL = 1,
    CHANNEL_INVALID = 2,
    PERMISSION_DENIED = 3
  };

  // our ports
  enum port_types {
    RX_PORT = 0,
    TX_PORT = 1
  };
  static const int N_PORTS = 4;
  std::vector<mb_port_sptr> d_tx, d_rx;
  mb_port_sptr	d_cs;

  static const int D_USB_CAPACITY = 32 * 1024 * 1024;
  static const int D_MAX_CHANNELS = 16;
  long d_ntx_chan;
  long d_nrx_chan;

  struct channel_info {
    long assigned_capacity;  // the capacity currently assignedby the channel
    pmt_t owner;              // port ID of the owner of the channel
  };

  struct channel_info d_chaninfo_tx[D_MAX_CHANNELS];
  struct channel_info d_chaninfo_rx[D_MAX_CHANNELS];

public:
  usrp_server(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg);
  ~usrp_server();

  void initial_transition();
  void handle_message(mb_message_sptr msg);

protected:
  static int max_capacity() { return D_USB_CAPACITY; }

private:
  void handle_cmd_allocate_channel(pmt_t port_id, pmt_t data);
  void handle_cmd_deallocate_channel(pmt_t port_id, pmt_t data);
  void handle_cmd_xmit_raw_frame(pmt_t data);
  int rx_port_index(pmt_t port_id);
  int tx_port_index(pmt_t port_id);
  long current_capacity_allocation();
};

#endif /* INCLUDED_USRP_SERVER_H */
