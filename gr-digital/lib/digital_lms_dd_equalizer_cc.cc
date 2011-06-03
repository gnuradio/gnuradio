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

#include <digital_lms_dd_equalizer_cc.h>
#include <gr_io_signature.h>
#include <gr_misc.h>
#include <iostream>

digital_lms_dd_equalizer_cc_sptr
digital_make_lms_dd_equalizer_cc (float mu, int ntaps,
				  digital_constellation_sptr cnst)
{
  return gnuradio::get_initial_sptr(new digital_lms_dd_equalizer_cc (mu, ntaps, cnst));
}

digital_lms_dd_equalizer_cc::digital_lms_dd_equalizer_cc (float mu, int ntaps,
							  digital_constellation_sptr cnst)
  : gr_sync_block ("lms_dd_equalizer_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex))),
    d_taps(ntaps), d_cnst(cnst)
{
  set_mu(mu);

  gr_zero_vector(d_taps);
  d_taps [d_taps.size()/2] = 1;

  set_history(ntaps+1);
}

float
digital_lms_dd_equalizer_cc::get_mu()
{
  return d_mu;
}

void
digital_lms_dd_equalizer_cc::set_mu(float mu)
{
  if(mu < 0.0f || mu > 1.0f) {
    throw std::out_of_range("digital_lms_dd_equalizer::set_mu: Gain value must in [0, 1]");
  }
  else {
    d_mu = mu;
  }
}

int
digital_lms_dd_equalizer_cc::work (int noutput_items,
				   gr_vector_const_void_star &input_items,
				   gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];
  
  gr_complex acc, decision, error;

  for(int i = 0; i < noutput_items; i++) {
    acc = 0;

    // Compute output
    for (size_t j=0; j < d_taps.size(); j++) 
      acc += in[i+j] * conj(d_taps[j]);
    
    d_cnst->map_to_points(d_cnst->decision_maker(&acc), &decision);
    error = decision - acc;
    
    //  Update taps
    for (size_t j=0; j < d_taps.size(); j++)
      d_taps[j] += d_mu * conj(error) * in[i+j];
    
    out[i] = acc;
  }

  return noutput_items;
}
