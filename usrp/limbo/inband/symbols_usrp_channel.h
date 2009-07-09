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
#ifndef INCLUDED_SYMBOLS_USRP_CHANNEL_H
#define INCLUDED_SYMBOLS_USRP_CHANNEL_H

#include <pmt.h>

// Outgoing
static pmt_t s_cmd_allocate_channel = pmt_intern("cmd-allocate-channel");
static pmt_t s_cmd_deallocate_channel = pmt_intern("cmd-deallocate-channel");

// Incoming
static pmt_t s_response_allocate_channel = pmt_intern("response-allocate-channel");
static pmt_t s_response_deallocate_channel = pmt_intern("response-deallocate-channel");

// Errors
static pmt_t s_err_requested_capacity_unavailable = pmt_intern("err-requested-capacity-unavailable");
static pmt_t s_err_channel_unavailable = pmt_intern("err-channel-unavailable");
static pmt_t s_err_channel_invalid = pmt_intern("err-channel-invalid");
static pmt_t s_err_channel_permission_denied = pmt_intern("err-channel-permission-denied");

#endif /* INCLUDED_SYMBOLS_USRP_CHANNEL_H */
