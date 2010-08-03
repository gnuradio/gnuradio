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

#include <gr_single_pole_iir_filter_ff.h>
#include <gr_io_signature.h>
#include <stdio.h>


gr_single_pole_iir_filter_ff_sptr 
gr_make_single_pole_iir_filter_ff (double alpha, unsigned int vlen)
{
  return gnuradio::get_initial_sptr(new gr_single_pole_iir_filter_ff(alpha, vlen));
}

gr_single_pole_iir_filter_ff::gr_single_pole_iir_filter_ff (
      		                    double alpha, unsigned int vlen)
  : gr_sync_block ("single_pole_iir_filter_ff",
		   gr_make_io_signature (1, 1, sizeof (float) * vlen),
		   gr_make_io_signature (1, 1, sizeof (float) * vlen)),
    d_vlen(vlen), d_iir(vlen)
{
  set_taps(alpha);
}

gr_single_pole_iir_filter_ff::~gr_single_pole_iir_filter_ff ()
{
  // nop
}

void
gr_single_pole_iir_filter_ff::set_taps (double alpha)
{
  for (unsigned int i = 0; i < d_vlen; i++)
    d_iir[i].set_taps(alpha);
}

int
gr_single_pole_iir_filter_ff::work (int noutput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];
  unsigned int vlen = d_vlen;

  if (d_vlen == 1){
    for (int i = 0; i < noutput_items; i++)
      out[i] = d_iir[0].filter (in[i]);
  }
  else {
    for (int i = 0; i < noutput_items; i++){
      for (unsigned int j = 0; j < vlen; j++){
	*out++ = d_iir[j].filter (*in++);
      }
    }
  }
  return noutput_items;
};
