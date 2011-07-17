/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#include <digital_constellation_decoder_cb.h>
#include <digital_constellation.h>
#include <gr_io_signature.h>
#include <iostream>

digital_constellation_decoder_cb_sptr
digital_make_constellation_decoder_cb (digital_constellation_sptr constellation)
{
  return gnuradio::get_initial_sptr
    (new digital_constellation_decoder_cb(constellation));
}

digital_constellation_decoder_cb::
digital_constellation_decoder_cb (digital_constellation_sptr constellation)
  : gr_block ("constellation_decoder_cb",
	      gr_make_io_signature (1, 1, sizeof (gr_complex)),
	      gr_make_io_signature (1, 1, sizeof (unsigned char))),
    d_constellation(constellation),
    d_dim(constellation->dimensionality())
{
  set_relative_rate (1.0 / ((double) d_dim));
}

void
digital_constellation_decoder_cb::forecast (int noutput_items,
					    gr_vector_int &ninput_items_required)
{
  unsigned int input_required = noutput_items * d_dim;

  unsigned ninputs = ninput_items_required.size();
  for (unsigned int i = 0; i < ninputs; i++)
    ninput_items_required[i] = input_required;
}


int
digital_constellation_decoder_cb::general_work (int noutput_items,
						gr_vector_int &ninput_items,
						gr_vector_const_void_star &input_items,
						gr_vector_void_star &output_items)
{
  gr_complex const *in = (const gr_complex *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];

  for(int i = 0; i < noutput_items; i++){
    out[i] = d_constellation->decision_maker(&(in[i*d_dim]));
  }

  consume_each (noutput_items * d_dim);
  return noutput_items;
}
