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

#include <gr_int_to_float.h>
#include <gr_io_signature.h>
#include <volk/volk.h>

gr_int_to_float_sptr
gr_make_int_to_float (size_t vlen, float scale)
{
  return gnuradio::get_initial_sptr(new gr_int_to_float (vlen, scale));
}

gr_int_to_float::gr_int_to_float (size_t vlen, float scale)
  : gr_sync_block ("gr_int_to_float",
		   gr_make_io_signature (1, 1, sizeof (int32_t)*vlen),
		   gr_make_io_signature (1, 1, sizeof (float)*vlen)),
    d_vlen(vlen), d_scale(scale)
{
  const int alignment_multiple =
    volk_get_alignment() / sizeof(float);
  set_alignment(alignment_multiple);
}

int
gr_int_to_float::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const int32_t *in = (const int32_t *) input_items[0];
  float *out = (float *) output_items[0];
  
  if(is_unaligned()) {
    volk_32i_s32f_convert_32f_u(out, in, d_scale, d_vlen*noutput_items);
  }
  else {
    volk_32i_s32f_convert_32f_a(out, in, d_scale, d_vlen*noutput_items);
  }

  return noutput_items;
}



