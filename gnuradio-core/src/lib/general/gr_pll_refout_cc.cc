/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_pll_refout_cc.h>
#include <gr_io_signature.h>
#include <gr_sincos.h>
#include <math.h>
#include <gr_math.h>

#define M_TWOPI (2*M_PI)

gr_pll_refout_cc_sptr
gr_make_pll_refout_cc (float alpha, float beta, float max_freq, float min_freq)
{
  return gr_pll_refout_cc_sptr (new gr_pll_refout_cc (alpha, beta, max_freq, min_freq));
}

gr_pll_refout_cc::gr_pll_refout_cc (float alpha, float beta, float max_freq, float min_freq)
  : gr_sync_block ("pll_refout_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex))),
    d_alpha(alpha), d_beta(beta), 
    d_max_freq(max_freq), d_min_freq(min_freq),
    d_phase(0), d_freq((max_freq+min_freq)/2)
{
}

float
gr_pll_refout_cc::mod_2pi (float in)
{
  if(in>M_PI)
    return in-M_TWOPI;
  else if(in<-M_PI)
    return in+M_TWOPI;
  else
    return in;
}

float
gr_pll_refout_cc::phase_detector(gr_complex sample,float ref_phase)
{
  float sample_phase;
  sample_phase = gr_fast_atan2f(sample.imag(),sample.real());
  return mod_2pi(sample_phase-ref_phase);
}

int
gr_pll_refout_cc::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  const gr_complex *iptr = (gr_complex *) input_items[0];
  gr_complex *optr = (gr_complex *) output_items[0];

  float error;
  float t_imag, t_real;
  int	size = noutput_items;
  
  while (size-- > 0) {
    error = phase_detector(*iptr++,d_phase);
    
    d_freq = d_freq + d_beta * error;
    d_phase = mod_2pi(d_phase + d_freq + d_alpha * error);
    
    if (d_freq > d_max_freq)
      d_freq = d_max_freq;
    else if (d_freq < d_min_freq)
      d_freq = d_min_freq;
    gr_sincosf(d_phase,&t_imag,&t_real);
    *optr++ = gr_complex(t_real,t_imag);
  }
  return noutput_items;
}
