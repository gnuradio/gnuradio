/* -*- c++ -*- */
/*
 * Copyright 2003,2009 Free Software Foundation, Inc.
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

#include <fusb.h>

// ------------------------------------------------------------------------
// 			     device handle
// ------------------------------------------------------------------------

fusb_devhandle::fusb_devhandle (libusb_device_handle *udh)
  : d_udh (udh)
{
  // that's it
};

fusb_devhandle::~fusb_devhandle ()
{
  // nop
}

// ------------------------------------------------------------------------
// 			     end point handle
// ------------------------------------------------------------------------

fusb_ephandle::fusb_ephandle (int endpoint, bool input_p,
			      int block_size, int nblocks)
  : d_endpoint (endpoint), d_input_p (input_p),
    d_block_size (block_size), d_nblocks (nblocks), d_started (false)
{
  // that't it
}

fusb_ephandle::~fusb_ephandle ()
{
  // nop
}
