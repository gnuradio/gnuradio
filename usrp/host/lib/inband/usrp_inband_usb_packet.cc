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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp_inband_usb_packet.h>

#include <usrp_bytesex.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

/*!
 * \brief Aligns the packet payload on a 32 bit boundary.  This is essential to
 * all control/status packets so that the inband FPGA code can parse them
 * easily.
 *
 * \returns true if successful or if the packet was already aligned; false if it
 * cannot be aligned.
 */
bool usrp_inband_usb_packet::align32()
{
  int p_len = payload_len();

  int bytes_needed = 4 - (p_len % 4);

  if(bytes_needed == 4)
    return true;

  // If the room left in the packet is less than the number of bytes
  // needed, return false to indicate no room to align
  if((MAX_PAYLOAD - p_len) < bytes_needed)
    return false;
    
  incr_header_len(bytes_needed);

  return true;
}

/*!
 * \brief Adds a ping command to the current control packet.
 *
 * The \p rid is the rid to be associated with the ping response and \p ping_val
 * is currently unused.
 *
 * \returns true if adding the ping command was successful, false otherwise
 * (i.e. no space in the current packet).
 */
bool usrp_inband_usb_packet::cs_ping(long rid, long ping_val)
{
  if(!align32())
    return false;
  
  int p_len = payload_len();

  if((MAX_PAYLOAD - p_len) < (CS_PING_LEN + CS_FIXED_LEN))
    return false;

  uint32_t ping = ( 
      ((OP_PING_FIXED & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((CS_PING_LEN & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((rid & CS_RID_MASK) << CS_RID_SHIFT)
    | (ping_val & CS_PINGVAL_MASK)

    );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(ping);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + CS_PING_LEN);

  return true;
}

/*!
 * \brief Adds a ping response to the packet.  This is used by the fake USRP
 * code to generate fake responses for pings.
 *
 * The \p rid is the RID to be associated with the response and \p ping_val is
 * currently unused.
 *
 * \returns true if the ping reply was added successfully, false otherwise.
 */
bool usrp_inband_usb_packet::cs_ping_reply(long rid, long ping_val) 
{
  if(!align32())
    return false;

  int p_len = payload_len();

  if((MAX_PAYLOAD - p_len) < (CS_PING_LEN + CS_FIXED_LEN))
    return false;

  uint32_t ping = ( 
      ((OP_PING_FIXED_REPLY & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((CS_PING_LEN & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((rid & CS_RID_MASK) << CS_RID_SHIFT)
    | ((ping_val & CS_PINGVAL_MASK) << CS_PINGVAL_SHIFT)

    );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(ping);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + CS_PING_LEN);

  return true;
}

/*!
 * \brief Adds a write register command to the packet.
 *
 * The \p reg_num is the register number for which the value \p val will be
 * written to.
 *
 * \returns true if the command was added to the packet successfully, false
 * otherwise.
 */
bool usrp_inband_usb_packet::cs_write_reg(long reg_num, long val)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  if((MAX_PAYLOAD - p_len) < (CS_WRITEREG_LEN + CS_FIXED_LEN))
    return false;

  uint32_t word0 = 0;

  // Build the first word which includes the register number
  word0 = (
      ((OP_WRITE_REG & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((CS_WRITEREG_LEN & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((reg_num & CS_REGNUM_MASK) << CS_REGNUM_SHIFT)
  );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word0);

  // The second word is solely the register value to be written
  // FIXME: should this be unsigned?
  payload += 1; 
  *payload = host_to_usrp_u32((uint32_t) val);
  
  // Rebuild the header to update the payload length
  incr_header_len(CS_FIXED_LEN + CS_WRITEREG_LEN);

  return true;
}

/*!
 * \brief Adds a write register masked command to the packet.
 *
 * The \p reg_num is the register number for which the value \p val will be
 * written, masked by \p mask
 *
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_write_reg_masked(long reg_num, long val, long mask)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  if((MAX_PAYLOAD - p_len) < (CS_WRITEREGMASKED_LEN + CS_FIXED_LEN))
    return false;

  uint32_t word0 = 0;

  // Build the first word which includes the register number
  word0 = (
      ((OP_WRITE_REG_MASKED & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((CS_WRITEREGMASKED_LEN & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((reg_num & CS_REGNUM_MASK) << CS_REGNUM_SHIFT)
  );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word0);

  // Skip over the first word and write the register value
  payload += 1;
  *payload = host_to_usrp_u32((uint32_t) val);

  // Skip over the register value and write the mask
  payload += 1;
  *payload = host_to_usrp_u32((uint32_t) mask);
  
  // Rebuild the header to update the payload length
  incr_header_len(CS_FIXED_LEN + CS_WRITEREGMASKED_LEN);

  return true;
}

/*!
 * \brief Adds a read register message to the packet. 
 *
 * The \p rid will be the associated RID returned with the response, and \p
 * reg_num is the register to be read.
 * 
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_read_reg(long rid, long reg_num)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  if((MAX_PAYLOAD - p_len) < (CS_READREG_LEN + CS_FIXED_LEN))
    return false;

  uint32_t read_reg = ( 
      ((OP_READ_REG & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((CS_READREG_LEN & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((rid & CS_RID_MASK) << CS_RID_SHIFT)
    | ((reg_num & CS_REGNUM_MASK) << CS_REGNUM_SHIFT)

    );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(read_reg);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + CS_READREG_LEN);

  return true;
}

/*!
 * \brief Adds a read register reply response to the current packet.  This is
 * used by the fake USRP code to generate fake register read responses for
 * testing.
 *
 * The \p rid is the associated RID to be included in the response, \p reg_num
 * is the register the read is coming from, and \p reg_val is the value of the
 * read.
 *
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_read_reg_reply(long rid, long reg_num, long reg_val)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  if((MAX_PAYLOAD - p_len) < (CS_READREGREPLY_LEN + CS_FIXED_LEN))
    return false;

  uint32_t word0 = ( 
      ((OP_READ_REG_REPLY & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((CS_READREGREPLY_LEN & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((rid & CS_RID_MASK) << CS_RID_SHIFT)
    | ((reg_num & CS_REGNUM_MASK) << CS_REGNUM_SHIFT)

    );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word0);

  // Hop to the next word and write the reg value
  payload += 1;
  *payload = host_to_usrp_u32((uint32_t) reg_val); 

  // Update payload length
  incr_header_len(CS_FIXED_LEN + CS_READREGREPLY_LEN);

  return true;
}

/*!
 * \brief Adds a delay command to the current packet.
 *
 * The \p ticks parameter is the number of clock ticks the FPGA should delay
 * parsing for, which is added to the packet.
 *
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_delay(long ticks)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  if((MAX_PAYLOAD - p_len) < (CS_DELAY_LEN + CS_FIXED_LEN))
    return false;

  uint32_t delay = ( 
      ((OP_DELAY & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((CS_DELAY_LEN & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((ticks & CS_DELAY_MASK) << CS_DELAY_SHIFT)

    );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(delay);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + CS_DELAY_LEN);

  return true;
}

/*!
 * \brief
 *
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_i2c_write(long i2c_addr, uint8_t *i2c_data, size_t data_len)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  int i2c_len = data_len + 2;   // 2 bytes between mbz and addr

  if((MAX_PAYLOAD - p_len) < (i2c_len + CS_FIXED_LEN))
    return false;

  uint32_t word0 = 0;

  word0 = (
      ((OP_I2C_WRITE & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((i2c_len & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((i2c_addr & CS_I2CADDR_MASK) << CS_I2CADDR_SHIFT)
  );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
   *payload = host_to_usrp_u32(word0);

   // Jump over the first word and write the data
   // FIXME: Should the data be changed to usrp byte order?
   payload += 1;
   memcpy(payload, i2c_data, data_len);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + i2c_len);

  return true;
}

/*!
 * \brief Adds an I2C read command to the current packet.
 *
 * The \p rid is the associated RID to return with the read response, \p
 * i2c_addr is the address to read from on the I2C bus, and \p n_bytes is the
 * number of bytes to be read from the bus.
 *
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_i2c_read(long rid, long i2c_addr, long n_bytes)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  if((MAX_PAYLOAD - p_len) < (CS_I2CREAD_LEN + CS_FIXED_LEN))
    return false;

  uint32_t word0 = 0;
  
  word0 = ( 
      ((OP_I2C_READ & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((CS_I2CREAD_LEN & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((rid & CS_RID_MASK) << CS_RID_SHIFT)
    | ((i2c_addr & CS_I2CADDR_MASK) << CS_I2CADDR_SHIFT)
    );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word0);

  // Jump a word and write the number of bytes to read
  payload += 1;
  uint32_t word1 = 
    (n_bytes & CS_I2CREADBYTES_MASK) << CS_I2CREADBYTES_SHIFT;
  *payload = host_to_usrp_u32(word1);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + CS_I2CREAD_LEN);

  return true;
}

/*!
 * \brief Adds an I2C read reply response to the current packet.  This is used
 * by the fake USRP code to generate fake I2C responses.
 *
 * The \p rid is the RID to be associated with the response, \p i2c_addr is the
 * address on the I2C bus that the \p i2c_data of \p i2c_data_len was read from.
 *
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_i2c_read_reply(long rid, long i2c_addr, uint8_t *i2c_data, long i2c_data_len)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  int i2c_len = i2c_data_len + 2;

  if((MAX_PAYLOAD - p_len) < (i2c_len + CS_FIXED_LEN)) 
    return false;
  
  uint32_t word0 = 0;
  
  word0 = ( 
      ((OP_I2C_READ_REPLY & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((i2c_len & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((rid & CS_RID_MASK) << CS_RID_SHIFT)
    | ((i2c_addr & CS_I2CADDR_MASK) << CS_I2CADDR_SHIFT)
    );

  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word0);

  // Jump a word and write the actual data
  payload += 1;
  memcpy(payload, i2c_data, i2c_data_len);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + i2c_len);

  return true;
}

/*!
 * \brief Adds a SPI write command to the current packet.
 *
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_spi_write(long enables, long format, long opt_header_bytes, uint8_t *spi_data, long spi_data_len)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  int spi_len = spi_data_len + 6;

  if((MAX_PAYLOAD - p_len) < (spi_len + CS_FIXED_LEN))
    return false;

  uint32_t word = 0;

  // First word contains the opcode and length, then mbz
  word = (
      ((OP_SPI_WRITE & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((spi_len & CS_LEN_MASK) << CS_LEN_SHIFT)
    );
  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word);

  payload += 1;

  // Second word contains the enables, format, and optional tx bytes
  word = 0;
  word = (
      ((enables & CS_SPIENABLES_MASK) << CS_SPIENABLES_SHIFT)
    | ((format & CS_SPIFORMAT_MASK) << CS_SPIFORMAT_SHIFT)
    | ((opt_header_bytes & CS_SPIOPT_MASK) << CS_SPIOPT_SHIFT)
    );
  payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word);

  payload += 1;
  memcpy(payload, spi_data, spi_data_len);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + spi_len);

  return true;
}

/*!
 * \brief Adds a SPI bus read command to the packet.
 *
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_spi_read(long rid, long enables, long format, long opt_header_bytes, long n_bytes)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  if((MAX_PAYLOAD - p_len) < (CS_SPIREAD_LEN + CS_FIXED_LEN))
    return false;

  uint32_t word = 0;

  // First word contains the opcode, length, and RID
  word = (
      ((OP_SPI_READ & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((CS_SPIREAD_LEN & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((rid & CS_RID_MASK) << CS_RID_SHIFT)
    );
  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word);

  payload += 1;

  // Second word contains the enables, format, and optional tx bytes
  word = 0;
  word = (
      ((enables & CS_SPIENABLES_MASK) << CS_SPIENABLES_SHIFT)
    | ((format & CS_SPIFORMAT_MASK) << CS_SPIFORMAT_SHIFT)
    | ((opt_header_bytes & CS_SPIOPT_MASK) << CS_SPIOPT_SHIFT)
    );
  payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word);

  payload += 1;

  // The third word contains the number of bytes
  word = 0;
  word = (
      ((n_bytes & CS_SPINBYTES_MASK) << CS_SPINBYTES_SHIFT)
    );
  payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + CS_SPIREAD_LEN);

  return true;
}

/*!
 * \brief Adds an SPI read reply to the current packet.  This is used by the
 * fake USRP code to generate fake responses for SPI reads.
 *
 * \returns true if the command was added to the packet, false otherwise.
 */
bool usrp_inband_usb_packet::cs_spi_read_reply(long rid, uint8_t *spi_data, long spi_data_len)
{
  if(!align32())
    return false;

  int p_len = payload_len();

  int spi_len = spi_data_len + 2;

  if((MAX_PAYLOAD - p_len) < (spi_len + CS_FIXED_LEN))
    return false;

  uint32_t word = 0;

  // First word contains the opcode, length, and RID
  word = (
      ((OP_SPI_READ_REPLY & CS_OPCODE_MASK) << CS_OPCODE_SHIFT)
    | ((spi_len & CS_LEN_MASK) << CS_LEN_SHIFT)
    | ((rid & CS_RID_MASK) << CS_RID_SHIFT)
    );
  uint32_t *payload = (uint32_t *) (d_payload + p_len);
  *payload = host_to_usrp_u32(word);

  // Jump a word and write the actual data
  payload += 1;
  memcpy(payload, spi_data, spi_data_len);

  // Update payload length
  incr_header_len(CS_FIXED_LEN + spi_len);

  return true;
}

/*!
 * \brief Since all control packets contain subpackets which have the length of
 * the subpacket at a uniform location in the subpacket, this will return the
 * subpacket length given a byte offset of the start of the subpacket from the beginning of the packet.
 *
 * \returns the length of the subpacket
 */
int usrp_inband_usb_packet::cs_len(int payload_offset) {
  uint32_t subpkt = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset)));
  return (subpkt >> CS_LEN_SHIFT) & CS_LEN_MASK;
}

/*!
 * \brief The following method takes an offset within the packet payload to
 * extract a control/status subpacket and constructs a pmt response which
 * includes the proper signal and arguments specified by usrp-low-level-cs.  The
 * USRP server could therefore use this to read subpackets and pass them
 * responses back up to the application.  It's arguable that only reply packets
 * should be parsed here, however we parse others for use in debugging or
 * failure reporting on the transmit side of packets.
 */
pmt_t usrp_inband_usb_packet::read_subpacket(int payload_offset) {

  uint32_t subpkt = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset)));
  uint32_t opcode = (subpkt >> CS_OPCODE_SHIFT) & CS_OPCODE_MASK;
  uint32_t len = (subpkt >> CS_LEN_SHIFT) & CS_LEN_MASK;

  switch(opcode) {
    
    case OP_PING_FIXED_REPLY:
    {
      pmt_t rid     = pmt_from_long((subpkt >> CS_RID_SHIFT) & CS_RID_MASK);
      pmt_t pingval = pmt_from_long((subpkt >> CS_PINGVAL_SHIFT) & CS_PINGVAL_MASK);
      return pmt_list3(s_op_ping_fixed_reply, rid, pingval);
    }

    case OP_READ_REG_REPLY:
    {
      pmt_t rid     = pmt_from_long((subpkt >> CS_RID_SHIFT) & CS_RID_MASK);
      pmt_t reg_num = pmt_from_long((subpkt >> CS_REGNUM_SHIFT) & CS_REGNUM_MASK);

      // To get the register value we just read the next 32 bits
      uint32_t val  = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset + 4)));
      pmt_t reg_val = pmt_from_long(val);

      return pmt_list4(s_op_read_reg_reply, rid, reg_num, reg_val);
    }

    case OP_I2C_READ_REPLY:
    {
      pmt_t rid       = pmt_from_long((subpkt >> CS_RID_SHIFT) & CS_RID_MASK);
      pmt_t i2c_addr  = pmt_from_long((subpkt >> CS_I2CADDR_SHIFT) & CS_I2CADDR_MASK);

      // Make a u8 vector to dump the data from the packet into
      size_t i2c_data_len;
      pmt_t i2c_data  = pmt_make_u8vector(len - 2, 0);   // skip rid+mbz+addr = 2 bytes
      uint8_t *w_data  = 
          (uint8_t *) pmt_u8vector_writable_elements(i2c_data, i2c_data_len);

      memcpy(w_data, d_payload + payload_offset + 4, i2c_data_len);  // skip first word

      return pmt_list4(s_op_i2c_read_reply, rid, i2c_addr, i2c_data);
    }

    case OP_SPI_READ_REPLY:
    {
      pmt_t rid       = pmt_from_long((subpkt >> CS_RID_SHIFT) & CS_RID_MASK);
      
      // Make a u8 vector to dump the data from the packet into
      size_t spi_data_len;
      pmt_t spi_data  = pmt_make_u8vector(len - 2, 0);   // skip rid+mbz+addr = 2 bytes
      uint8_t *w_data  = 
          (uint8_t *) pmt_u8vector_writable_elements(spi_data, spi_data_len);

      memcpy(w_data, d_payload + payload_offset + 4, spi_data_len);  // skip first word

      return pmt_list3(s_op_spi_read_reply, rid, spi_data);
    }

    case OP_PING_FIXED:
    {
      pmt_t rid     = pmt_from_long((subpkt >> CS_RID_SHIFT) & CS_RID_MASK);
      pmt_t pingval = pmt_from_long((subpkt >> CS_PINGVAL_SHIFT) & CS_PINGVAL_MASK);
      return pmt_list3(s_op_ping_fixed, rid, pingval);
    }

    case OP_WRITE_REG:
    {
      pmt_t reg_num = pmt_from_long((subpkt >> CS_REGNUM_SHIFT) & CS_REGNUM_MASK);

      // To get the register value we just read the next 32 bits
      uint32_t val  = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset + 4)));
      pmt_t reg_val = pmt_from_long(val);

      return pmt_list3(s_op_write_reg, reg_num, reg_val);
    }

    case OP_WRITE_REG_MASKED:
    {
      pmt_t reg_num = pmt_from_long((subpkt >> CS_REGNUM_SHIFT) & CS_REGNUM_MASK);

      // To get the register value we just read the next 32 bits
      uint32_t val  = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset + 4)));
      pmt_t reg_val = pmt_from_long(val);

      // The mask is the next 32 bits
      uint32_t mask  = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset + 8)));
      pmt_t reg_mask = pmt_from_long(mask);

      return pmt_list4(s_op_write_reg_masked, reg_num, reg_val, reg_mask);
    }

    case OP_READ_REG:
    {
      pmt_t rid     = pmt_from_long((subpkt >> CS_RID_SHIFT) & CS_RID_MASK);
      pmt_t reg_num = pmt_from_long((subpkt >> CS_REGNUM_SHIFT) & CS_REGNUM_MASK);

      return pmt_list3(s_op_read_reg, rid, reg_num);
    }

    case OP_I2C_WRITE:
    {
      pmt_t i2c_addr    = pmt_from_long((subpkt >> CS_I2CADDR_SHIFT) & CS_I2CADDR_MASK);

      // The length includes an extra 2 bytes for storing the mbz and addr
      pmt_t i2c_data    = pmt_make_u8vector(len-2, 0);

      // Get a writable address to copy the data from the packet
      size_t ignore;
      uint8_t *w_data = (uint8_t *) pmt_u8vector_writable_elements(i2c_data, ignore);
      memcpy(w_data, d_payload + payload_offset + 4, len-2);

      
      return pmt_list3(s_op_i2c_write, i2c_addr, i2c_data);
    }

    case OP_I2C_READ:
    {
      pmt_t rid       = pmt_from_long((subpkt >> CS_RID_SHIFT) & CS_RID_MASK);
      pmt_t i2c_addr  = pmt_from_long((subpkt >> CS_I2CADDR_SHIFT) & CS_I2CADDR_MASK);
      
      // The number of bytes is in the next word
      uint32_t bytes  = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset + 4)));
      bytes = (bytes >> CS_I2CREADBYTES_SHIFT) & CS_I2CREADBYTES_MASK;
      pmt_t i2c_bytes = pmt_from_long(bytes);

      return pmt_list4(s_op_i2c_read, rid, i2c_addr, i2c_bytes);
    }

    case OP_SPI_WRITE:
    {
      // Nothing interesting in the first word, skip to the next
      uint32_t word  = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset + 4)));
      pmt_t enables   = pmt_from_long((word >> CS_SPIENABLES_SHIFT) & CS_SPIENABLES_MASK);
      pmt_t format    = pmt_from_long((word >> CS_SPIFORMAT_SHIFT) & CS_SPIFORMAT_MASK);
      pmt_t opt       = pmt_from_long((word >> CS_SPIOPT_SHIFT) & CS_SPIOPT_MASK);

      // From the next word and on is data
      size_t spi_data_len;
      pmt_t spi_data  = pmt_make_u8vector(len - 6, 0);   // skip rid+mbz+addr = 2 bytes
      uint8_t *w_data  = 
          (uint8_t *) pmt_u8vector_writable_elements(spi_data, spi_data_len);

      memcpy(w_data, d_payload + payload_offset + 8, spi_data_len);  // skip first 2 words

      return pmt_list5(s_op_spi_write, enables, format, opt, spi_data);
    }

    case OP_SPI_READ:
    {
      // Read the RID from the first word, the rest is mbz
      pmt_t rid       = pmt_from_long((subpkt >> CS_RID_SHIFT) & CS_RID_MASK);

      // Continue at the next word...
      uint32_t word  = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset + 4)));
      pmt_t enables   = pmt_from_long((word >> CS_SPIENABLES_SHIFT) & CS_SPIENABLES_MASK);
      pmt_t format    = pmt_from_long((word >> CS_SPIFORMAT_SHIFT) & CS_SPIFORMAT_MASK);
      pmt_t opt       = pmt_from_long((word >> CS_SPIOPT_SHIFT) & CS_SPIOPT_MASK);

      // The number of bytes is the only thing to read in the next word
      word  = usrp_to_host_u32(*((uint32_t *)(d_payload + payload_offset + 8)));
      pmt_t n_bytes   = pmt_from_long((word >> CS_SPINBYTES_SHIFT) & CS_SPINBYTES_MASK);

      return pmt_list6(s_op_spi_read, rid, enables, format, opt, n_bytes);
    }

    case OP_DELAY:
    {
      pmt_t ticks = pmt_from_long((subpkt >> CS_DELAY_SHIFT) & CS_DELAY_MASK);

      return pmt_list2(s_op_delay, ticks);
    }
    
    default:
      return PMT_NIL;

  }
}

