/* -*- c++ -*- */
/*
 * Copyright 2005,2010 Free Software Foundation, Inc.
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

#include <gr_uchar_to_float.h>
#include <gr_io_signature.h>
#include <gri_uchar_to_float.h>

gr_uchar_to_float_sptr
gr_make_uchar_to_float ()
{
  return gnuradio::get_initial_sptr(new gr_uchar_to_float ());
}

gr_uchar_to_float::gr_uchar_to_float ()
  : gr_sync_block ("gr_uchar_to_float",
		   gr_make_io_signature (1, 1, sizeof (unsigned char)),
		   gr_make_io_signature (1, 1, sizeof (float)))
{
}

int
gr_uchar_to_float::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *) input_items[0];
  float *out = (float *) output_items[0];

  gri_uchar_to_float (in, out, noutput_items);
  
  return noutput_items;
}
