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
#ifndef INCLUDED_SYMBOLS_USRP_RX_H
#define INCLUDED_SYMBOLS_USRP_RX_H

#include <pmt.h>

// Outgoing
static pmt_t s_cmd_start_recv_raw_samples = pmt_intern("cmd-start-recv-raw-samples");
static pmt_t s_cmd_stop_recv_raw_samples = pmt_intern("cmd-stop-recv-raw-samples");

// Incoming
static pmt_t s_response_recv_raw_samples = pmt_intern("response-recv-raw-samples");

// Errors
static pmt_t s_err_already_receiving = pmt_intern("err-already-receiving");

#endif /* INCLUDED_SYMBOLS_USRP_RX_H */
