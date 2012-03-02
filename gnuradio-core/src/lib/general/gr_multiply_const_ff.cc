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

#include <gr_multiply_const_ff.h>
#include <gr_io_signature.h>
#include <volk/volk.h>

gr_multiply_const_ff_sptr
gr_make_multiply_const_ff (float k, size_t vlen)
{
  return gnuradio::get_initial_sptr(new gr_multiply_const_ff (k, vlen));
}

gr_multiply_const_ff::gr_multiply_const_ff (float k, size_t vlen)
  : gr_sync_block ("gr_multiply_const_ff",
		   gr_make_io_signature (1, 1, sizeof (float)*vlen),
		   gr_make_io_signature (1, 1, sizeof (float)*vlen)),
    d_k(k), d_vlen(vlen)
{
 const int alignment_multiple =
   volk_get_alignment() / sizeof(float);
 set_alignment(alignment_multiple);
}

float
gr_multiply_const_ff::k() const
{
  return d_k;
}

void
gr_multiply_const_ff::set_k(float k)
{
  d_k = k;
}

int
gr_multiply_const_ff::work (int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];
  int noi = d_vlen*noutput_items;

  if(is_unaligned()) {
    volk_32f_s32f_multiply_32f_u(out, in, d_k, noi);
  }
  else {
    volk_32f_s32f_multiply_32f_a(out, in, d_k, noi);
  }

  return noutput_items;
}



