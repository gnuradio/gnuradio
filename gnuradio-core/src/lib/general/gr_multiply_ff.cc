/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gr_multiply_ff.h>
#include <gr_io_signature.h>
#include <volk/volk.h>

gr_multiply_ff_sptr
gr_make_multiply_ff (size_t vlen)
{
  return gnuradio::get_initial_sptr(new gr_multiply_ff (vlen));
}

gr_multiply_ff::gr_multiply_ff (size_t vlen)
  : gr_sync_block ("gr_multiply_ff",
		   gr_make_io_signature (1, -1, sizeof (float)*vlen),
		   gr_make_io_signature (1, 1, sizeof (float)*vlen)),
    d_vlen(vlen)
{
  const int alignment_multiple = 
    volk_get_alignment() / sizeof(float);
  set_alignment(alignment_multiple);
}

int
gr_multiply_ff::work (int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  float *out = (float *) output_items[0];
  int noi = d_vlen*noutput_items;

  memcpy(out, input_items[0], noi*sizeof(float));
  if(is_unaligned()) {
    for(size_t i = 1; i < input_items.size(); i++)
      volk_32f_x2_multiply_32f_u(out, out, (const float*)input_items[i], noi);
  }
  else {
    for(size_t i = 1; i < input_items.size(); i++)
      volk_32f_x2_multiply_32f_a(out, out, (const float*)input_items[i], noi);
  }
  return noutput_items;
}



