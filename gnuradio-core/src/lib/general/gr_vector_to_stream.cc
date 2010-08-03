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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_vector_to_stream.h>
#include <gr_io_signature.h>
#include <string.h>

gr_vector_to_stream_sptr
gr_make_vector_to_stream (size_t item_size, size_t nitems_per_block)
{
  return gnuradio::get_initial_sptr(new gr_vector_to_stream (item_size, nitems_per_block));
}

gr_vector_to_stream::gr_vector_to_stream (size_t item_size, size_t nitems_per_block)
  : gr_sync_interpolator ("vector_to_stream",
			  gr_make_io_signature (1, 1, item_size * nitems_per_block),
			  gr_make_io_signature (1, 1, item_size),
			  nitems_per_block)
{
}

int
gr_vector_to_stream::work (int noutput_items,
			     gr_vector_const_void_star &input_items,
			     gr_vector_void_star &output_items)
{
  size_t block_size = output_signature()->sizeof_stream_item (0);

  const char *in = (const char *) input_items[0];
  char *out = (char *) output_items[0];

  memcpy (out, in, noutput_items * block_size);

  return noutput_items;
}
