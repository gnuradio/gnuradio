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
#ifndef INCLUDED_SYMBOLS_USRP_LOW_LEVEL_CS_H
#define INCLUDED_SYMBOLS_USRP_LOW_LEVEL_CS_H

#include <pmt.h>

// Outgoing
static pmt_t s_cmd_to_control_channel = pmt_intern("cmd-to-control-channel");

// Incoming
static pmt_t s_response_from_control_channel = pmt_intern("response-from-control-channel");

// Subpackets
static pmt_t s_op_ping_fixed = pmt_intern("op-ping-fixed");
static pmt_t s_op_ping_fixed_reply = pmt_intern("op-ping-fixed-reply");
static pmt_t s_op_write_reg = pmt_intern("op-write-reg");
static pmt_t s_op_write_reg_masked = pmt_intern("op-write-reg-masked");
static pmt_t s_op_read_reg = pmt_intern("op-read-reg");
static pmt_t s_op_read_reg_reply = pmt_intern("op-read-reg-reply");
static pmt_t s_op_i2c_write = pmt_intern("op-i2c-write");
static pmt_t s_op_i2c_read = pmt_intern("op-i2c-read");
static pmt_t s_op_i2c_read_reply = pmt_intern("op-i2c-read-reply");
static pmt_t s_op_spi_write = pmt_intern("op-spi-write");
static pmt_t s_op_spi_read = pmt_intern("op-spi-read");
static pmt_t s_op_spi_read_reply = pmt_intern("op-spi-read-reply");
static pmt_t s_op_delay = pmt_intern("op-delay");

#endif /* INCLUDED_SYMBOLS_USRP_LOW_LEVEL_CS_H */
