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

#include <gr_pll_freqdet_cf.h>
#include <gr_io_signature.h>
#include <math.h>
#include <gr_math.h>

#ifndef M_TWOPI
#define M_TWOPI (2.0f*M_PI)
#endif

gr_pll_freqdet_cf_sptr
gr_make_pll_freqdet_cf (float loop_bw, float max_freq, float min_freq)
{
  return gnuradio::get_initial_sptr(new gr_pll_freqdet_cf (loop_bw, max_freq, min_freq));
}

gr_pll_freqdet_cf::gr_pll_freqdet_cf (float loop_bw, float max_freq, float min_freq)
  : gr_sync_block ("pll_freqdet_cf",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (float))),
    gri_control_loop(loop_bw, max_freq, min_freq)    
{
}

float
gr_pll_freqdet_cf::mod_2pi (float in)
{
  if(in>M_PI)
    return in-M_TWOPI;
  else if(in<-M_PI)
    return in+M_TWOPI;
  else
    return in;
}

float
gr_pll_freqdet_cf::phase_detector(gr_complex sample,float ref_phase)
{
  float sample_phase;
  sample_phase = gr_fast_atan2f(sample.imag(),sample.real());
  return mod_2pi(sample_phase-ref_phase);
}

int
gr_pll_freqdet_cf::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  const gr_complex *iptr = (gr_complex *) input_items[0];
  float *optr = (float *) output_items[0];

  float error;
  int	size = noutput_items;
  
  while (size-- > 0) {
    *optr++ = d_freq;

    error = phase_detector(*iptr++,d_phase);
    
    advance_loop(error);
    phase_wrap();
    frequency_limit();
  }
  return noutput_items;
}
