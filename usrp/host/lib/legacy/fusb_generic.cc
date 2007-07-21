/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

#include <fusb_generic.h>
#include <usb.h>


static const int USB_TIMEOUT = 1000;	// in milliseconds


fusb_devhandle_generic::fusb_devhandle_generic (usb_dev_handle *udh)
  : fusb_devhandle (udh)
{
  // that's it
}

fusb_devhandle_generic::~fusb_devhandle_generic ()
{
  // nop
}

fusb_ephandle *
fusb_devhandle_generic::make_ephandle (int endpoint, bool input_p,
				       int block_size, int nblocks)
{
  return new fusb_ephandle_generic (this, endpoint, input_p,
				    block_size, nblocks);
}

// ----------------------------------------------------------------

fusb_ephandle_generic::fusb_ephandle_generic (fusb_devhandle_generic *dh,
					      int endpoint, bool input_p,
					      int block_size, int nblocks)
  : fusb_ephandle (endpoint, input_p, block_size, nblocks),
    d_devhandle (dh)
{
  // that's it
}

fusb_ephandle_generic::~fusb_ephandle_generic ()
{
  // nop
}

bool
fusb_ephandle_generic::start ()
{
  d_started = true;
  return true;
}

bool
fusb_ephandle_generic::stop ()
{
  d_started = false;
  return true;
}

int
fusb_ephandle_generic::write (const void *buffer, int nbytes)
{
  if (!d_started)	// doesn't matter here, but keeps semantics constant
    return -1;
  
  if (d_input_p)
    return -1;
  
  return usb_bulk_write (d_devhandle->get_usb_dev_handle (),
			 d_endpoint, (char *) buffer, nbytes, USB_TIMEOUT);
}

int
fusb_ephandle_generic::read (void *buffer, int nbytes)
{
  if (!d_started)	// doesn't matter here, but keeps semantics constant
    return -1;

  if (!d_input_p)
    return -1;

  return usb_bulk_read (d_devhandle->get_usb_dev_handle (),
			d_endpoint|USB_ENDPOINT_IN, (char *) buffer, nbytes,
			USB_TIMEOUT);
}
