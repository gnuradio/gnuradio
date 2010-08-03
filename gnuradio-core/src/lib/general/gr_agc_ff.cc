/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2010 Free Software Foundation, Inc.
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

#include <gr_agc_ff.h>
#include <gr_io_signature.h>
#include <gri_agc_ff.h>

gr_agc_ff_sptr
gr_make_agc_ff (float rate, float reference, float gain, float max_gain)
{
  return gnuradio::get_initial_sptr(new gr_agc_ff (rate, reference, gain, max_gain));
}

gr_agc_ff::gr_agc_ff (float rate, float reference, float gain, float max_gain)
  : gr_sync_block ("gr_agc_ff",
		   gr_make_io_signature (1, 1, sizeof (float)),
		   gr_make_io_signature (1, 1, sizeof (float)))
  , gri_agc_ff (rate,  reference, gain, max_gain)
{
}

int
gr_agc_ff::work (int noutput_items,
		 gr_vector_const_void_star &input_items,
		 gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];
  scaleN (out, in, noutput_items);
  return noutput_items;
}
