/* -*- c++ -*- */
/*
 * Copyright 2006, 2011 Free Software Foundation, Inc.
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

#include <gr_constellation_decoder2_cb.h>
#include <gr_constellation.h>
#include <gr_io_signature.h>

gr_constellation_decoder2_cb_sptr
gr_make_constellation_decoder2_cb (gr_constellation_sptr constellation)
{
  return gr_constellation_decoder2_cb_sptr 
		(new gr_constellation_decoder2_cb(constellation));
}

gr_constellation_decoder2_cb::
gr_constellation_decoder2_cb (gr_constellation_sptr constellation)
  : gr_sync_block ("constellation_decoder2_cb",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (unsigned char))),
    d_constellation(constellation)
{
}


gr_constellation_decoder2_cb::~gr_constellation_decoder2_cb(){}

int
gr_constellation_decoder2_cb::work(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  gr_complex const *in = (const gr_complex *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];
    
  for(int i = 0; i < noutput_items; i++){
    out[i] = d_constellation->decision_maker(in[i]);
  }

  return noutput_items;
}
