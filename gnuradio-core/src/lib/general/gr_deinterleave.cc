/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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

#include <gr_deinterleave.h>
#include <gr_io_signature.h>
#include <string.h>


gr_deinterleave_sptr
gr_make_deinterleave (size_t itemsize)
{
  return gnuradio::get_initial_sptr(new gr_deinterleave (itemsize));
}

gr_deinterleave::gr_deinterleave (size_t itemsize)
  : gr_sync_decimator ("deinterleave",
		       gr_make_io_signature (1, 1, itemsize),
		       gr_make_io_signature (1, gr_io_signature::IO_INFINITE, itemsize),
		       1),
    d_itemsize (itemsize)
{
}

gr_deinterleave::~gr_deinterleave ()
{
  // NOP
}

bool
gr_deinterleave::check_topology (int ninputs, int noutputs)
{
  set_decimation (noutputs);
  return true;
}

int
gr_deinterleave::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  size_t nchan = output_items.size ();
  size_t itemsize = d_itemsize;
  const char *in = (const char *) input_items[0];
  char **out = (char **) &output_items[0];

  for (int i = 0; i < noutput_items; i++){
    for (unsigned int n = 0; n < nchan; n++){
      memcpy (out[n], in, itemsize);
      out[n] += itemsize;
      in += itemsize;
    }
  }

  return noutput_items;
}
