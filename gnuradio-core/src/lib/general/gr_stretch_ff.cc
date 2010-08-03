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

#include <gr_stretch_ff.h>
#include <gr_io_signature.h>

gr_stretch_ff_sptr
gr_make_stretch_ff(float lo, size_t vlen)
{
  return gnuradio::get_initial_sptr(new gr_stretch_ff(lo, vlen));
}

gr_stretch_ff::gr_stretch_ff(float lo, size_t vlen)
  : gr_sync_block("stretch_ff",
		  gr_make_io_signature(1, 1, vlen * sizeof(float)),
		  gr_make_io_signature(1, 1, vlen * sizeof(float))),
    d_lo(lo), d_vlen(vlen)
{
}

int
gr_stretch_ff::work(int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  const float *in  = (const float *) input_items[0];
  float       *out = (float *) output_items[0];
  
  for (int count = 0; count < noutput_items; count++) {
    float vmax = in[0] - d_lo;

    for (unsigned int i = 1; i < d_vlen; i++) {
      float vtmp = in[i] - d_lo;
      if (vtmp > vmax)
	vmax = vtmp;
    }
    
    if (vmax != 0.0)
      for (unsigned int i = 0; i < d_vlen; i++)
	out[i] = d_lo * (1.0 - (in[i] - d_lo) / vmax);
    else
      for (unsigned int i = 0; i < d_vlen; i++)
	out[i] = in[i];

    in  += d_vlen;
    out += d_vlen;
  }
  
  return noutput_items;
}

