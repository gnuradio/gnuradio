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
#ifndef INCLUDED_SYMBOLS_USRP_INTERFACE_CS_H
#define INCLUDED_SYMBOLS_USRP_INTERFACE_CS_H

#include <pmt.h>

// Outgoing
static pmt_t s_cmd_usrp_open = pmt_intern("cmd-usrp-open");
static pmt_t s_cmd_usrp_close = pmt_intern("cmd-usrp-close");
static pmt_t s_cmd_usrp_ntx_chan = pmt_intern("cmd-usrp-ntx-chan");
static pmt_t s_cmd_usrp_nrx_chan = pmt_intern("cmd-usrp-nrx-chan");
static pmt_t s_cmd_usrp_write = pmt_intern("cmd-usrp-write");
static pmt_t s_cmd_usrp_start_reading = pmt_intern("cmd-usrp-start-reading");
static pmt_t s_cmd_usrp_stop_reading = pmt_intern("cmd-usrp-stop-reading");

// Incoming
static pmt_t s_response_usrp_open = pmt_intern("response-usrp-open");
static pmt_t s_response_usrp_close = pmt_intern("response-usrp-close");
static pmt_t s_response_usrp_ntx_chan = pmt_intern("response-usrp-ntx-chan");
static pmt_t s_response_usrp_nrx_chan = pmt_intern("response-usrp-nrx-chan");
static pmt_t s_response_usrp_write = pmt_intern("response-usrp-write");
static pmt_t s_response_usrp_read = pmt_intern("response-usrp-read");

#endif /* INCLUDED_SYMBOLS_USRP_INTERFACE_CS_H */
