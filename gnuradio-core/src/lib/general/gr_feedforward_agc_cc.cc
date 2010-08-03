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

#include <gr_feedforward_agc_cc.h>
#include <gr_io_signature.h>
#include <stdexcept>

gr_feedforward_agc_cc_sptr
gr_make_feedforward_agc_cc(int nsamples, float reference)
{
  return gnuradio::get_initial_sptr(new gr_feedforward_agc_cc (nsamples, reference));
}

gr_feedforward_agc_cc::gr_feedforward_agc_cc (int nsamples, float reference)
  : gr_sync_block ("gr_feedforward_agc_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex))),
    d_nsamples(nsamples), d_reference(reference)
{
  if (nsamples < 1)
    throw std::invalid_argument("gr_feedforward_agc_cc: nsamples must be >= 1");
  
  set_history(nsamples);
}

gr_feedforward_agc_cc::~gr_feedforward_agc_cc()
{
}

inline static float
mag_squared(gr_complex x)
{
  return x.real() * x.real() + x.imag() * x.imag();
}

// approximate sqrt(x^2 + y^2)
inline static float
envelope(gr_complex x)
{
  float r_abs = std::fabs(x.real());
  float i_abs = std::fabs(x.imag());

  if (r_abs > i_abs)
    return r_abs + 0.4 * i_abs;
  else
    return i_abs + 0.4 * r_abs;
}

int
gr_feedforward_agc_cc::work(int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];
  int	nsamples = d_nsamples;
  float gain;
  
  for (int i = 0; i < noutput_items; i++){
    //float max_env = 1e-12;	// avoid divide by zero
    float max_env = 1e-4;	// avoid divide by zero, indirectly set max gain
    for (int j = 0; j < nsamples; j++)
      max_env = std::max(max_env, envelope(in[i+j]));
    gain = d_reference / max_env;
    out[i] = gain * in[i];
  }
  return noutput_items;
}
