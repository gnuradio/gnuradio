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

#include <gr_float_to_char.h>
#include <gr_io_signature.h>
#include <gri_float_to_char.h>
#include <volk/volk.h>

gr_float_to_char_sptr
gr_make_float_to_char ()
{
  return gnuradio::get_initial_sptr(new gr_float_to_char ());
}

gr_float_to_char::gr_float_to_char ()
  : gr_sync_block ("gr_float_to_char",
		   gr_make_io_signature (1, 1, sizeof (float)),
		   gr_make_io_signature (1, 1, sizeof (char)))
{
 const int alignment_multiple =
   volk_get_alignment() / sizeof(char);
 set_alignment(alignment_multiple);
}

int
gr_float_to_char::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
#if 1
  float d_scale = 1.0;
  const float *in = (const float *) input_items[0];
  int8_t *out = (int8_t *) output_items[0];

  if(is_unaligned()) {
    volk_32f_s32f_convert_8i_u(out, in, d_scale, noutput_items);
  }
  else {
    volk_32f_s32f_convert_8i_a(out, in, d_scale, noutput_items);
  }
#else
  const float *in = (const float *) input_items[0];
  char *out = (char *) output_items[0];
  gri_float_to_char (in, out, noutput_items);
#endif
  
  return noutput_items;
}



