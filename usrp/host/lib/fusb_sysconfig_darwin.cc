/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <fusb.h>
#include <fusb_darwin.h>

static const int MAX_BLOCK_SIZE = 32 * 1024;		// hard limit

fusb_devhandle *
fusb_sysconfig::make_devhandle (usb_dev_handle *udh)
{
  return new fusb_devhandle_darwin (udh);
}

int fusb_sysconfig::max_block_size ()
{
  return MAX_BLOCK_SIZE;
}
