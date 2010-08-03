/* -*- c++ -*- */
/*
 * Copyright 2008,2010 Free Software Foundation, Inc.
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

#include <gr_repeat.h>
#include <gr_io_signature.h>
#include <string.h>

gr_repeat_sptr 
gr_make_repeat(size_t itemsize, int interp)
{
  return gnuradio::get_initial_sptr(new gr_repeat(itemsize, interp));
}

gr_repeat::gr_repeat(size_t itemsize, int interp)  
  : gr_sync_interpolator("extend",
			 gr_make_io_signature(1, 1, itemsize),
			 gr_make_io_signature(1, 1, itemsize),
			 interp),
    d_interp(interp),
    d_itemsize(itemsize)
{
}

gr_repeat::~gr_repeat()
{
}

int 
gr_repeat::work(int noutput_items,
		  gr_vector_const_void_star &input_items,
		  gr_vector_void_star &output_items)
{
  const char *in = (const char *) input_items[0];
  char *out = (char *)output_items[0];

  for (int i = 0; i < noutput_items/d_interp; i++) {
    for (int j = 0; j < d_interp; j++) {
      memcpy(out, in, d_itemsize);
      out += d_itemsize;
    }

    in += d_itemsize;
  }

  return noutput_items;
}
