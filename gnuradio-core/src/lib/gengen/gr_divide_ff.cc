/* -*- c++ -*- */
/*
 * Copyright 2004, 2009,2010 Free Software Foundation, Inc.
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

#include <gr_divide_ff.h>
#include <gr_io_signature.h>

gr_divide_ff_sptr
gr_make_divide_ff (size_t vlen)
{
  return gnuradio::get_initial_sptr (new gr_divide_ff (vlen));
}

gr_divide_ff::gr_divide_ff (size_t vlen)
  : gr_sync_block ("divide_ff",
		   gr_make_io_signature (1, -1, sizeof (float)*vlen),
		   gr_make_io_signature (1,  1, sizeof (float)*vlen)),
  d_vlen (vlen)
{
}

int
gr_divide_ff::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  float *optr = (float *) output_items[0];

  int ninputs = input_items.size ();

  if (ninputs == 1){		// compute reciprocal
    for (size_t i = 0; i < noutput_items*d_vlen; i++)
      *optr++ = (float) ((float) 1 /
			    ((float *) input_items[0])[i]);
  }

  else {
    for (size_t i = 0; i < noutput_items*d_vlen; i++){
      float acc = ((float *) input_items[0])[i];
      for (int j = 1; j < ninputs; j++)
	acc /= ((float *) input_items[j])[i];

      *optr++ = (float) acc;
    }
  }

  return noutput_items;
}
