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

#include <gr_hilbert_fc.h>
#include <gr_firdes.h>
#include <gr_fir_fff.h>
#include <gr_fir_util.h>

// public constructor
gr_hilbert_fc_sptr 
gr_make_hilbert_fc (unsigned int ntaps)
{
  return gnuradio::get_initial_sptr(new gr_hilbert_fc (ntaps));
}

gr_hilbert_fc::gr_hilbert_fc (unsigned int ntaps)
  : gr_sync_block ("hilbert_fc",
		   gr_make_io_signature (1, 1, sizeof (float)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex))),
    d_ntaps (ntaps | 0x1),	// ensure ntaps is odd
    d_hilb (gr_fir_util::create_gr_fir_fff (gr_firdes::hilbert (d_ntaps)))
{
  set_history (d_ntaps);
}

gr_hilbert_fc::~gr_hilbert_fc ()
{
  delete d_hilb;
}

int
gr_hilbert_fc::work (int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  float *in = (float *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  for (int i = 0; i < noutput_items; i++)
    out[i] = gr_complex (in[i + d_ntaps/2],
			 d_hilb->filter (&in[i]));

  return noutput_items;
}
