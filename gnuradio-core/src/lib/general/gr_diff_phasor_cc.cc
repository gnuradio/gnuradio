/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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

#include <gr_diff_phasor_cc.h>
#include <gr_io_signature.h>

gr_diff_phasor_cc_sptr
gr_make_diff_phasor_cc ()
{
  return gnuradio::get_initial_sptr(new gr_diff_phasor_cc());
}

gr_diff_phasor_cc::gr_diff_phasor_cc ()
  : gr_sync_block ("diff_phasor_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex)))	
{
  set_history(2);
}


gr_diff_phasor_cc::~gr_diff_phasor_cc(){}

int
gr_diff_phasor_cc::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  gr_complex const *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];
  in += 1; // ensure that i - 1 is valid.

  for(int i = 0; i < noutput_items; i++){
    out[i] = in[i] * conj(in[i-1]);
  }
    
  return noutput_items;
}
