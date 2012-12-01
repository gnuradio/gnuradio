/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_message_debug.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <iostream>

// public constructor that returns a shared_ptr

gr_message_debug_sptr
gr_make_message_debug ()
{
  return gnuradio::get_initial_sptr(new gr_message_debug());
}

void gr_message_debug::print(pmt::pmt_t msg){
    std::cout << "******* MESSAGE DEBUG PRINT ********\n";
    pmt::pmt_print(msg);
    std::cout << "************************************\n";
}


gr_message_debug::gr_message_debug ()
  : gr_block("message_debug",
	     gr_make_io_signature(0, 0, 0),
	     gr_make_io_signature(0, 0, 0))
{
    message_port_register_in(pmt::mp("print"));
    set_msg_handler(pmt::mp("print"), boost::bind(&gr_message_debug::print, this, _1));
}

gr_message_debug::~gr_message_debug()
{
}
