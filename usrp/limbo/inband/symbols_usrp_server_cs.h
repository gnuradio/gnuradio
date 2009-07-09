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
#ifndef INCLUDED_SYMBOLS_USRP_SERVER_CS_H
#define INCLUDED_SYMBOLS_USRP_SERVER_CS_H

#include <pmt.h>

// Outgoing
static pmt_t s_cmd_open = pmt_intern("cmd-open");
static pmt_t s_cmd_close = pmt_intern("cmd-close");
static pmt_t s_cmd_max_capacity  = pmt_intern("cmd-max-capacity");
static pmt_t s_cmd_ntx_chan  = pmt_intern("cmd-ntx-chan");
static pmt_t s_cmd_nrx_chan  = pmt_intern("cmd-nrx-chan");
static pmt_t s_cmd_current_capacity_allocation  = pmt_intern("cmd-current-capacity-allocation");

// Incoming
static pmt_t s_response_open = pmt_intern("response-open");
static pmt_t s_response_close = pmt_intern("response-close");
static pmt_t s_response_max_capacity = pmt_intern("response-max-capacity");
static pmt_t s_response_ntx_chan = pmt_intern("response-ntx-chan");
static pmt_t s_response_nrx_chan = pmt_intern("response-nrx-chan");
static pmt_t s_response_current_capacity_allocation  = pmt_intern("response-current-capacity-allocation");

// Errors
static pmt_t s_err_usrp_not_opened = pmt_intern("err-usrp-not-opened");
static pmt_t s_err_usrp_already_opened = pmt_intern("err-usrp-already-opened");
static pmt_t s_err_usrp_already_closed = pmt_intern("err-usrp-already-closed");

#endif /* INCLUDED_SYMBOLS_USRP_SERVER_CS_H */
