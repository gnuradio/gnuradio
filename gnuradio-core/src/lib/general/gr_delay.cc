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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_delay.h>
#include <gr_io_signature.h>
#include <string.h>

gr_delay_sptr
gr_make_delay (size_t itemsize, int delay)
{
  return gr_delay_sptr (new gr_delay (itemsize, delay));
}

gr_delay::gr_delay (size_t itemsize, int delay)
  : gr_sync_block ("delay",
		   gr_make_io_signature (1, -1, itemsize),
		   gr_make_io_signature (1, -1, itemsize)),
    d_itemsize(itemsize)
{
  set_delay(delay);
}

int
gr_delay::work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
  assert(input_items.size() == output_items.size());

  const char *iptr;
  char *optr;

  for(size_t i = 0; i < input_items.size(); i++) {
    iptr = (const char *) input_items[i];
    optr = (char *) output_items[i];

    memcpy(optr, iptr, noutput_items*d_itemsize);
  }
  
  return noutput_items;
}
