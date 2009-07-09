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

#ifndef INCLUDED_USRP_INBAND_USB_PACKET_H_
#define INCLUDED_USRP_INBAND_USB_PACKET_H_

#include <usrp_bytesex.h>
#include <mblock/mblock.h>
#include <pmt.h>
#include <iostream>

#include <symbols_usrp_low_level_cs.h>

static const int USB_PKT_SIZE = 512;   // bytes
static const int MAX_PAYLOAD = USB_PKT_SIZE-2*sizeof(uint32_t);
static const int CONTROL_CHAN = 0x1f;

class usrp_inband_usb_packet {
  //
  // keep raw packet in USRP-endian order
  //
  uint32_t	      d_word0;
  uint32_t	      d_timestamp;
  unsigned char	  d_payload[MAX_PAYLOAD];

public:

  enum opcodes {
    OP_PING_FIXED         = 0x00,
    OP_PING_FIXED_REPLY   = 0x01,
    OP_WRITE_REG          = 0x02,
    OP_WRITE_REG_MASKED   = 0x03,
    OP_READ_REG           = 0x04,
    OP_READ_REG_REPLY     = 0x05,
    OP_I2C_WRITE          = 0x06,
    OP_I2C_READ           = 0x07,
    OP_I2C_READ_REPLY     = 0x08,
    OP_SPI_WRITE          = 0x09,
    OP_SPI_READ           = 0x0a,
    OP_SPI_READ_REPLY     = 0x0b,
    OP_DELAY              = 0x0c
  };

  enum flags {
    FL_OVERRUN        = 0x80000000,
    FL_UNDERRUN       = 0x40000000,
    FL_DROPPED        = 0x20000000,
    FL_START_OF_BURST = 0x10000000,
    FL_END_OF_BURST   = 0x08000000,
    FL_CARRIER_SENSE  = 0x04000000,

    FL_ALL_FLAGS      = 0xfc000000
  };

  static const int FL_OVERRUN_SHIFT = 31;
  static const int FL_UNDERRUN_SHIFT = 30;
  static const int FL_DROPPED_SHIFT = 29;
  static const int FL_END_OF_BURST_SHIFT = 27;
  static const int FL_START_OF_BURST_SHIFT = 28;
  
  static const int RSSI_MASK = 0x3f;
  static const int RSSI_SHIFT = 21;

  static const int CHAN_MASK = 0x1f;
  static const int CHAN_SHIFT = 16;

  static const int TAG_MASK = 0xf;
  static const int TAG_SHIFT = 9;

  static const int PAYLOAD_LEN_MASK = 0x1ff;
  static const int PAYLOAD_LEN_SHIFT = 0;

  // Fixed size for opcode and length fields
  static const int CS_FIXED_LEN = 2;

  static const int CS_OPCODE_MASK = 0xff;
  static const int CS_OPCODE_SHIFT = 24;

  static const int CS_LEN_MASK = 0xff;
  static const int CS_LEN_SHIFT = 16;

  static const int CS_RID_MASK = 0x3f;
  static const int CS_RID_SHIFT = 10;

  static const int CS_PING_LEN = 2;
  static const int CS_PINGVAL_MASK = 0x3ff;
  static const int CS_PINGVAL_SHIFT = 0;

  static const int CS_WRITEREG_LEN = 6;
  static const int CS_WRITEREGMASKED_LEN = 10;
  static const int CS_READREG_LEN = 2;
  static const int CS_READREGREPLY_LEN = 6;
  static const int CS_REGNUM_MASK = 0x3ff;
  static const int CS_REGNUM_SHIFT = 0;

  static const int CS_DELAY_LEN = 2;
  static const int CS_DELAY_MASK = 0xffff;
  static const int CS_DELAY_SHIFT = 0;

  static const int CS_I2CADDR_MASK = 0x7f;
  static const int CS_I2CADDR_SHIFT = 0;

  static const int CS_I2CREAD_LEN = 3;
  static const int CS_I2CREADBYTES_MASK = 0x7f;
  static const int CS_I2CREADBYTES_SHIFT = 24;

  static const int CS_SPIOPT_MASK = 0xffff;
  static const int CS_SPIOPT_SHIFT = 0;
  static const int CS_SPIFORMAT_MASK = 0xff;
  static const int CS_SPIFORMAT_SHIFT = 16;
  static const int CS_SPIENABLES_MASK = 0xff;
  static const int CS_SPIENABLES_SHIFT = 24;
  static const int CS_SPIREAD_LEN = 7;
  static const int CS_SPINBYTES_MASK = 0xff;
  static const int CS_SPINBYTES_SHIFT = 24;

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

  void incr_header_len(int val) {
    set_header(flags(), chan(), tag(), payload_len() + val);
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

  static int max_pkt_size() {
    return USB_PKT_SIZE;
  }

  // C/S methods
  bool align32();
  bool cs_ping(long rid, long ping_val);
  bool cs_ping_reply(long rid, long ping_val);
  bool cs_write_reg(long reg_num, long val);
  bool cs_write_reg_masked(long reg_num, long val, long mask);
  bool cs_read_reg(long rid, long reg_num);
  bool cs_read_reg_reply(long rid, long reg_num, long reg_val);
  bool cs_delay(long ticks);
  bool cs_i2c_write(long i2c_addr, uint8_t *i2c_data, size_t data_len);
  bool cs_i2c_read(long rid, long i2c_addr, long n_bytes);
  bool cs_i2c_read_reply(long rid, long i2c_addr, uint8_t *i2c_data, long i2c_data_len);
  bool cs_spi_write(long enables, long format, long opt_header_bytes, uint8_t *spi_data, long spi_data_len);
  bool cs_spi_read(long rid, long enables, long format, long opt_header_bytes, long n_bytes);
  bool cs_spi_read_reply(long rid, uint8_t *spi_data, long spi_data_len);
  int cs_len(int payload_offset);
  pmt_t read_subpacket(int payload_offset);
};

#endif
