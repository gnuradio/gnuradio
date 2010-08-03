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

#include <gr_filter_delay_fc.h>
#include <gr_fir_fff.h>
#include <gr_fir_util.h>

// public constructor
gr_filter_delay_fc_sptr 
gr_make_filter_delay_fc (const std::vector<float> &taps) 
{
  return gnuradio::get_initial_sptr(new gr_filter_delay_fc (taps));
}

gr_filter_delay_fc::gr_filter_delay_fc (const std::vector<float> &taps)
  : gr_sync_block ("filter_delay_fc",
		   gr_make_io_signature (1, 2, sizeof (float)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex)))
{
  d_fir = gr_fir_util::create_gr_fir_fff (taps);
  d_delay = d_fir->ntaps () / 2;
  set_history (d_fir->ntaps ());
}

gr_filter_delay_fc::~gr_filter_delay_fc ()
{
  delete d_fir;
}

int
gr_filter_delay_fc::work (int noutput_items,
		     gr_vector_const_void_star &input_items,
		     gr_vector_void_star &output_items)
{
  float *in0  = (float *) input_items[0];
  float *in1  = (float *) input_items[1];
  gr_complex *out = (gr_complex *) output_items[0];

  switch (input_items.size ()){
  case 1:
    for (int i = 0; i < noutput_items; i++)
      out[i] = gr_complex (in0[i + d_delay], 
			   d_fir->filter (&in0[i]));
    break;

  case 2:
    for (int j = 0; j < noutput_items; j++)
      out[j] = gr_complex (in0[j + d_delay],
     			   d_fir->filter (&in1[j]));
    break;

  default:
    assert (0);
  }

  return noutput_items;
}
