/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2011 Free Software Foundation, Inc.
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

#include <gr_pll_refout_cc.h>
#include <gr_io_signature.h>
#include <gr_sincos.h>
#include <math.h>
#include <gr_math.h>

#ifndef M_TWOPI
#define M_TWOPI (2.0f*M_PI)
#endif

gr_pll_refout_cc_sptr
gr_make_pll_refout_cc (float loop_bw, float max_freq, float min_freq)
{
  return gnuradio::get_initial_sptr(new gr_pll_refout_cc (loop_bw, max_freq, min_freq));
}

gr_pll_refout_cc::gr_pll_refout_cc (float loop_bw, float max_freq, float min_freq)
  : gr_sync_block ("pll_refout_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex))),
    gri_control_loop(loop_bw, max_freq, min_freq)
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
    gr_sincosf(d_phase,&t_imag,&t_real);
    *optr++ = gr_complex(t_real,t_imag);

    error = phase_detector(*iptr++,d_phase);
    
    advance_loop(error);
    phase_wrap();
    frequency_limit();
  }
  return noutput_items;
}
