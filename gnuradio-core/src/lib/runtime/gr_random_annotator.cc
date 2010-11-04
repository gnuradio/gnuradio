/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <gr_random_annotator.h>
#include <gr_io_signature.h>

gr_random_annotator::gr_random_annotator (size_t sizeof_stream_item)
  : gr_sync_block ("random_annotator",
		   gr_make_io_signature (1, 1, sizeof_stream_item),
		   gr_make_io_signature (1, 1, sizeof_stream_item))
{
}

gr_random_annotator::~gr_random_annotator ()
{
}

gr_random_annotator_sptr
gr_make_random_annotator (size_t sizeof_stream_item)
{
  return gnuradio::get_initial_sptr (new gr_random_annotator (sizeof_stream_item));
}

int
gr_random_annotator::work (int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
{
  return noutput_items;
}
