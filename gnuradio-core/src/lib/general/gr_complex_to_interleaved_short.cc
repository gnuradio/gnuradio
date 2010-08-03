/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2010 Free Software Foundation, Inc.
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

#include <gr_complex_to_interleaved_short.h>
#include <gr_io_signature.h>
#include <math.h>

gr_complex_to_interleaved_short_sptr
gr_make_complex_to_interleaved_short ()
{
  return gnuradio::get_initial_sptr(new gr_complex_to_interleaved_short ());
}

gr_complex_to_interleaved_short::gr_complex_to_interleaved_short ()
  : gr_sync_interpolator ("gr_complex_to_interleaved_short",
			  gr_make_io_signature (1, 1, sizeof (gr_complex)),
			  gr_make_io_signature (1, 1, sizeof (short)),
			  2)
{
}

int
gr_complex_to_interleaved_short::work (int noutput_items,
				       gr_vector_const_void_star &input_items,
				       gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  short *out = (short *) output_items[0];

  for (int i = 0; i < noutput_items/2; i++){
    *out++ = (short) lrintf(in[i].real());	// FIXME saturate?
    *out++ = (short) lrintf(in[i].imag());
  }
  
  return noutput_items;
}



