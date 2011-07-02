/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010 Free Software Foundation, Inc.
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

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_multiply_const_vss.h>
#include <gr_io_signature.h>

gr_multiply_const_vss_sptr
gr_make_multiply_const_vss (const std::vector<short> &k)
{
  return gnuradio::get_initial_sptr (new gr_multiply_const_vss (k));
}

gr_multiply_const_vss::gr_multiply_const_vss (const std::vector<short> &k)
  : gr_sync_block ("multiply_const_vss",
		   gr_make_io_signature (1, 1, sizeof(short)*k.size()),
		   gr_make_io_signature (1, 1, sizeof(short)*k.size()))
{
  d_k = k;
}

int
gr_multiply_const_vss::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  short *iptr = (short *)input_items[0];
  short *optr = (short *)output_items[0];
 
  int nitems_per_block = output_signature()->sizeof_stream_item(0)/sizeof(short);

  for (int i = 0; i < noutput_items; i++)
    for (int j = 0; j < nitems_per_block; j++)
      *optr++ = *iptr++ * d_k[j];
  
  return noutput_items;
}
