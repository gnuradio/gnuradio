/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#include <gr_pll_carriertracking_cc.h>
#include <gr_io_signature.h>
#include <gr_sincos.h>
#include <math.h>
#include <gr_math.h>

#define M_TWOPI (2*M_PI)

gr_pll_carriertracking_cc_sptr
gr_make_pll_carriertracking_cc (float alpha, float beta, float max_freq, float min_freq)
{
  return gr_pll_carriertracking_cc_sptr (new gr_pll_carriertracking_cc (alpha, beta, max_freq, min_freq));
}

gr_pll_carriertracking_cc::gr_pll_carriertracking_cc (float alpha, float beta, float max_freq, float min_freq)
  : gr_sync_block ("pll_carriertracking_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex))),
    d_alpha(alpha), d_beta(beta), 
    d_max_freq(max_freq), d_min_freq(min_freq),
    d_phase(0), d_freq((max_freq+min_freq)/2),
    d_locksig(0),d_lock_threshold(0),d_squelch_enable(false)
{
}

float
gr_pll_carriertracking_cc::mod_2pi (float in)
{
  if(in>M_PI)
    return in-M_TWOPI;
  else if(in<-M_PI)
    return in+M_TWOPI;
  else
    return in;
}

float
gr_pll_carriertracking_cc::phase_detector(gr_complex sample,float ref_phase)
{
  float sample_phase;
  //  sample_phase = atan2(sample.imag(),sample.real());
  sample_phase = gr_fast_atan2f(sample.imag(),sample.real());
  return mod_2pi(sample_phase-ref_phase);
}

bool
gr_pll_carriertracking_cc::lock_detector(void)
{
    return (fabs(d_locksig) > d_lock_threshold);
}

bool
gr_pll_carriertracking_cc::squelch_enable(bool set_squelch)
{
    return d_squelch_enable = set_squelch;
}

float
gr_pll_carriertracking_cc::set_lock_threshold(float threshold)
{
    return d_lock_threshold = threshold;
}

int
gr_pll_carriertracking_cc::work (int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
{
  const gr_complex *iptr = (gr_complex *) input_items[0];
  gr_complex *optr = (gr_complex *) output_items[0];

  float error;
  float t_imag, t_real;
  
  for (int i = 0; i < noutput_items; i++){
    error = phase_detector(iptr[i],d_phase);
    
    d_freq = d_freq + d_beta * error;
    d_phase = mod_2pi(d_phase + d_freq + d_alpha * error);
    
    if (d_freq > d_max_freq)
      d_freq = d_max_freq;
    else if (d_freq < d_min_freq)
      d_freq = d_min_freq;
    gr_sincosf(d_phase,&t_imag,&t_real);
    optr[i] = iptr[i] * gr_complex(t_real,-t_imag);
    d_locksig = d_locksig * (1.0 - d_alpha) + d_alpha*(iptr[i].real() * t_real + iptr[i].imag() * t_imag);
    
    if ((d_squelch_enable) && !lock_detector())
      optr[i] = 0;
  }
  return noutput_items;
}
