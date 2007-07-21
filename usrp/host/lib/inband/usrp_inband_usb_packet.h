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

#ifndef INCLUDED_USRP_INBAND_USB_PACKET_H_
#define INCLUDED_USRP_INBAND_USB_PACKET_H_

#include <usrp_bytesex.h>
#include <mb_mblock.h>

static const int USB_PKT_SIZE = 512;   // bytes
static const int MAX_PAYLOAD = USB_PKT_SIZE-2*sizeof(uint32_t);

class usrp_inband_usb_packet {
  //
  // keep raw packet in USRP-endian order
  //
  uint32_t	      d_word0;
  uint32_t	      d_timestamp;
  unsigned char	  d_payload[MAX_PAYLOAD];

public:

  enum flags {
    FL_OVERRUN        = 0x80000000,
    FL_UNDERRUN       = 0x40000000,
    FL_DROPPED        = 0x20000000,
    FL_END_OF_BURST   = 0x10000000,
    FL_START_OF_BURST = 0x08000000,

    FL_ALL_FLAGS      = 0xf8000000
  };

  static const int FL_OVERRUN_SHIFT = 31;
  static const int FL_UNDERRUN_SHIFT = 30;
  static const int FL_DROPPED_SHIFT = 29;
  static const int FL_END_OF_BURST_SHIFT = 28;
  static const int FL_START_OF_BURST_SHIFT = 27;
  
  static const int RSSI_MASK = 0x3f;
  static const int RSSI_SHIFT = 21;

  static const int CHAN_MASK = 0x1f;
  static const int CHAN_SHIFT = 16;

  static const int TAG_MASK = 0xf;
  static const int TAG_SHIFT = 9;

  static const int PAYLOAD_LEN_MASK = 0x1ff;
  static const int PAYLOAD_LEN_SHIFT = 0;

public:
  
  void set_timestamp(uint32_t timestamp){
    d_timestamp = host_to_usrp_u32(timestamp);
  }

  void set_end_of_burst() {
    uint32_t word0 = usrp_to_host_u32(d_word0);
    word0 |= 1<<FL_END_OF_BURST_SHIFT;
    d_word0 = host_to_usrp_u32(word0);
  }

  void set_header(int flags, int chan, int tag, int payload_len){
    uint32_t word0 =  ((flags & FL_ALL_FLAGS)
                       | ((chan & CHAN_MASK) << CHAN_SHIFT)
                       | ((tag & TAG_MASK) << TAG_SHIFT)
                       | ((payload_len & PAYLOAD_LEN_MASK) << PAYLOAD_LEN_SHIFT));
    d_word0 = host_to_usrp_u32(word0);
  }
  
  uint32_t timestamp() const {
    return usrp_to_host_u32(d_timestamp);
  }

  int rssi() const {
    uint32_t word0 = usrp_to_host_u32(d_word0);
    return (word0 >> RSSI_SHIFT) & RSSI_MASK;
  }

  int chan() const {
    uint32_t word0 = usrp_to_host_u32(d_word0);
    return (word0 >> CHAN_SHIFT) & CHAN_MASK;
  }

  int tag() const {
    uint32_t word0 = usrp_to_host_u32(d_word0);
    return (word0 >> TAG_SHIFT) & TAG_MASK;
  }

  int payload_len() const {
    uint32_t word0 = usrp_to_host_u32(d_word0);
    return (word0 >> PAYLOAD_LEN_SHIFT) & PAYLOAD_LEN_MASK;
  }
  
  int flags() const {
    return usrp_to_host_u32(d_word0) & FL_ALL_FLAGS;
  }

  int overrun() const {
    return (usrp_to_host_u32(d_word0) & FL_OVERRUN) >> FL_OVERRUN_SHIFT;
  }
  

  int underrun() const {
    return (usrp_to_host_u32(d_word0) & FL_UNDERRUN) >> FL_UNDERRUN_SHIFT;
  }


  int start_of_burst() const {
    return (usrp_to_host_u32(d_word0) & FL_START_OF_BURST) >> FL_START_OF_BURST_SHIFT;
  }

  int end_of_burst() const {
    return (usrp_to_host_u32(d_word0) & FL_END_OF_BURST) >> FL_END_OF_BURST_SHIFT;
  }

  int dropped() const {
    return (usrp_to_host_u32(d_word0) & FL_DROPPED) >> FL_DROPPED_SHIFT;
  }

  unsigned char *payload() { 
    return d_payload; 
  }

  static int max_payload() {
    return MAX_PAYLOAD;
  }

};

#endif
