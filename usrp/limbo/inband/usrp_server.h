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
#ifndef INCLUDED_USRP_SERVER_H
#define INCLUDED_USRP_SERVER_H

#include <mblock/mblock.h>
#include <vector>
#include <queue>
#include <fstream>
#include <usrp_inband_usb_packet.h>

typedef usrp_inband_usb_packet transport_pkt;   // makes conversion to gigabit easy

/*!
 * \brief Implements the lowest-level mblock usb_interface to the USRP
 */
class usrp_server : public mb_mblock
{
public:

  // our ports
  enum port_types {
    RX_PORT = 0,
    TX_PORT = 1
  };
  static const int N_PORTS = 4;
  std::vector<mb_port_sptr> d_tx, d_rx;
  mb_port_sptr	d_cs;
  mb_port_sptr	d_cs_usrp;

  static const int D_USB_CAPACITY = 32 * 1024 * 1024;
  static const int D_MAX_CHANNELS = 16;
  long d_ntx_chan;
  long d_nrx_chan;

  pmt_t d_usrp_dict;

  bool d_fpga_debug;
  
  long d_interp_tx;
  long d_decim_rx;

  // Keep track of the request IDs
  struct rid_info {
    pmt_t owner;
    long user_rid;

    rid_info() {
      owner = PMT_NIL;
      user_rid = 0;
    }
  };

  static const long D_MAX_RID = 64;
  std::vector<rid_info> d_rids;
  
  struct channel_info {
    long assigned_capacity;   // the capacity currently assignedby the channel
    pmt_t owner;              // port ID of the owner of the channel

    channel_info() {
      assigned_capacity = 0;
      owner = PMT_NIL;
    }
  };

  long d_rx_chan_mask;    // A bitmask representing the channels in the
                          // receiving state

  std::vector<struct channel_info> d_chaninfo_tx;
  std::vector<struct channel_info> d_chaninfo_rx;

  std::queue<mb_message_sptr> d_defer_queue;

  bool d_defer;
  bool d_opened;

  bool d_fake_rx;

public:
  usrp_server(mb_runtime *rt, const std::string &instance_name, pmt_t user_arg);
  ~usrp_server();

  void initial_transition();
  void handle_message(mb_message_sptr msg);

protected:
  static int max_capacity() { return D_USB_CAPACITY; }

private:
  void handle_cmd_allocate_channel(mb_port_sptr port, std::vector<struct channel_info> &chan_info, pmt_t data);
  void handle_cmd_deallocate_channel(mb_port_sptr port, std::vector<struct channel_info> &chan_info, pmt_t data);
  void handle_cmd_xmit_raw_frame(mb_port_sptr port, std::vector<struct channel_info> &chan_info, pmt_t data);
  void handle_cmd_to_control_channel(mb_port_sptr port, std::vector<struct channel_info> &chan_info, pmt_t data);
  void handle_cmd_start_recv_raw_samples(mb_port_sptr port, std::vector<struct channel_info> &chan_info, pmt_t data);
  void handle_cmd_stop_recv_raw_samples(mb_port_sptr port, std::vector<struct channel_info> &chan_info, pmt_t data);
  int rx_port_index(pmt_t port_id);
  int tx_port_index(pmt_t port_id);
  long current_capacity_allocation();
  void recall_defer_queue();
  void reset_channels();
  void handle_response_usrp_read(pmt_t data);
  bool check_valid(mb_port_sptr port, long channel, std::vector<struct channel_info> &chan_info, pmt_t signal_info);
  void parse_control_pkt(pmt_t invocation_handle, transport_pkt *pkt);
  long next_rid();
  void initialize_registers();
  void set_register(long reg, long val);
  void read_register(long reg);
  void check_register_initialization();
  void reset_all_registers();
};

#endif /* INCLUDED_USRP_SERVER_H */
