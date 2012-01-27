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

#include <gr_float_to_short.h>
#include <gr_io_signature.h>
#include <gri_float_to_short.h>
#include <volk/volk.h>

gr_float_to_short_sptr
gr_make_float_to_short ()
{
  return gnuradio::get_initial_sptr(new gr_float_to_short ());
}

gr_float_to_short::gr_float_to_short ()
  : gr_sync_block ("gr_float_to_short",
		   gr_make_io_signature (1, 1, sizeof (float)),
		   gr_make_io_signature (1, 1, sizeof (short)))
{
 const int alignment_multiple =
   volk_get_alignment() / sizeof(short);
 set_alignment(alignment_multiple);
}

int
gr_float_to_short::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  short *out = (short *) output_items[0];

#if 1
  float d_scale = 1.0;
  //volk_32f_s32f_convert_16i_u(out, in, d_scale, noutput_items);
  if(is_unaligned()) {
    volk_32f_s32f_convert_16i_u(out, in, d_scale, noutput_items);
  }
  else {
    volk_32f_s32f_convert_16i_a(out, in, d_scale, noutput_items);
  }
#else
  gri_float_to_short (in, out, noutput_items);
#endif

  return noutput_items;
}



