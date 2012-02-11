/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#include <gr_float_to_int.h>
#include <gr_io_signature.h>
#include <gri_float_to_int.h>
#include <volk/volk.h>

gr_float_to_int_sptr
gr_make_float_to_int (size_t vlen, float scale)
{
  return gnuradio::get_initial_sptr(new gr_float_to_int (vlen, scale));
}

gr_float_to_int::gr_float_to_int (size_t vlen, float scale)
  : gr_sync_block ("gr_float_to_int",
		   gr_make_io_signature (1, 1, sizeof (float)*vlen),
		   gr_make_io_signature (1, 1, sizeof (int)*vlen)),
    d_vlen(vlen), d_scale(scale)
{
  const int alignment_multiple =
    volk_get_alignment() / sizeof(int);
  set_alignment(alignment_multiple);
}

float 
gr_float_to_int::scale() const
{
  return d_scale;
}

void
gr_float_to_int::set_scale(float scale)
{
  d_scale = scale;
}
int
gr_float_to_int::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  // Disable the Volk for now. There is a problem for large 32-bit ints that
  // are not properly represented by the precisions of a single float, which
  // can cause wrapping from large, positive numbers to negative.
  // In gri_float_to_int, the value is first promoted to a 64-bit
  // value, clipped, then converted to a float.
#if 0
  const float *in = (const float *) input_items[0];
  int32_t *out = (int32_t *) output_items[0];

  if(is_unaligned()) {
    volk_32f_s32f_convert_32i_u(out, in, d_scale, d_vlen*noutput_items);
  }
  else {
    volk_32f_s32f_convert_32i_a(out, in, d_scale, d_vlen*noutput_items);
  }
#else
  const float *in = (const float *) input_items[0];
  int *out = (int *) output_items[0];

  gri_float_to_int (in, out, d_scale, d_vlen*noutput_items);

#endif
  
  return noutput_items;
}
