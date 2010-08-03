/* -*- c++ -*- */
/*
 * Copyright 2005,2010 Free Software Foundation, Inc.
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

#include <gr_nlog10_ff.h>
#include <gr_io_signature.h>
#include <algorithm>

gr_nlog10_ff_sptr
gr_make_nlog10_ff (float n, unsigned vlen, float k)
{
  return gnuradio::get_initial_sptr(new gr_nlog10_ff(n, vlen, k));
}

gr_nlog10_ff::gr_nlog10_ff(float n, unsigned vlen, float k)
  : gr_sync_block("nlog10_ff",
		  gr_make_io_signature(1, 1, sizeof(float) * vlen),
		  gr_make_io_signature(1, 1, sizeof(float) * vlen)),
    d_vlen(vlen), d_n(n), d_k(k)
{
}

gr_nlog10_ff::~gr_nlog10_ff()
{
}

int
gr_nlog10_ff::work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];
  int noi = noutput_items * d_vlen;
  float n = d_n;
  float k = d_k;

  for (int i = 0; i < noi; i++)
    out[i] = n * log10(std::max(in[i], (float) 1e-18)) + k;

  return noutput_items;
}
