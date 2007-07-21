/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#include <gr_goertzel_fc.h>
#include <gr_io_signature.h>

// public constructor
gr_goertzel_fc_sptr 
gr_make_goertzel_fc(int rate, int len, float freq) 
{
  return gr_goertzel_fc_sptr (new gr_goertzel_fc(rate, len, freq));
}

gr_goertzel_fc::gr_goertzel_fc(int rate, int len, float freq)
  : gr_sync_decimator("goertzel_fc",
		      gr_make_io_signature (1, 1, sizeof (float)),
		      gr_make_io_signature (1, 1, sizeof (gr_complex)),
		      len),
    d_goertzel(rate, len, freq)
{
  d_len = len;
}

int gr_goertzel_fc::work(int noutput_items,
		         gr_vector_const_void_star &input_items,
		         gr_vector_void_star &output_items)
{
  float *in = (float *)input_items[0];
  gr_complex *out = (gr_complex *)output_items[0];

  for (int i = 0; i < noutput_items; i++) {
    *out++ = d_goertzel.batch(in);
    in += d_len;
  }

  return noutput_items;
}
