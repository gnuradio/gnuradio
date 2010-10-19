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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <@NAME@.h>
#include <gr_io_signature.h>

@SPTR_NAME@
gr_make_@BASE_NAME@ (const std::vector<@I_TYPE@> &k)
{
  return gnuradio::get_initial_sptr (new @NAME@ (k));
}

@NAME@::@NAME@ (const std::vector<@I_TYPE@> &k)
  : gr_sync_block ("@BASE_NAME@",
		   gr_make_io_signature (1, 1, sizeof(@I_TYPE@)*k.size()),
		   gr_make_io_signature (1, 1, sizeof(@O_TYPE@)*k.size()))
{
  d_k = k;
}

int
@NAME@::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  @I_TYPE@ *iptr = (@O_TYPE@ *)input_items[0];
  @O_TYPE@ *optr = (@O_TYPE@ *)output_items[0];
 
  int nitems_per_block = output_signature()->sizeof_stream_item(0)/sizeof(@I_TYPE@);

  for (int i = 0; i < noutput_items; i++)
    for (int j = 0; j < nitems_per_block; j++)
      *optr++ = *iptr++ * d_k[j];
  
  return noutput_items;
}
