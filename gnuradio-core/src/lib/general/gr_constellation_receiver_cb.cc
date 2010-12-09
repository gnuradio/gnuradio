/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2007,2010 Free Software Foundation, Inc.
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

#include <gr_io_signature.h>
#include <gr_prefs.h>
#include <gr_constellation_receiver_cb.h>
#include <stdexcept>
#include <gr_math.h>
#include <gr_expj.h>
// For debugging
#include <iostream>


#define M_TWOPI (2*M_PI)
#define VERBOSE_MM     0     // Used for debugging symbol timing loop
#define VERBOSE_COSTAS 0     // Used for debugging phase and frequency tracking

// Public constructor

gr_constellation_receiver_cb_sptr 
gr_make_constellation_receiver_cb(gr_constellation constell,
				  float alpha, float beta,
				  float fmin, float fmax)
{
  return gnuradio::get_initial_sptr(new gr_constellation_receiver_cb (constell, 
								      alpha, beta,
								      fmin, fmax));
}

gr_constellation_receiver_cb::gr_constellation_receiver_cb (gr_constellation constellation, 
							    float alpha, float beta,
							    float fmin, float fmax)
  : gr_block ("constellation_receiver_cb",
	      gr_make_io_signature (1, 1, sizeof (gr_complex)),
	      gr_make_io_signature (1, 1, sizeof (unsigned char))),
    d_constellation(constellation), 
    d_alpha(alpha), d_beta(beta), d_freq(0), d_max_freq(fmax), d_min_freq(fmin), d_phase(0),
    d_current_const_point(0)
{}

void
gr_constellation_receiver_cb::phase_error_tracking(float phase_error)
{
    
  d_freq += d_beta*phase_error;             // adjust frequency based on error
  d_phase += d_freq + d_alpha*phase_error;  // adjust phase based on error

  // Make sure we stay within +-2pi
  while(d_phase > M_TWOPI)
    d_phase -= M_TWOPI;
  while(d_phase < -M_TWOPI)
    d_phase += M_TWOPI;
  
  // Limit the frequency range
  d_freq = gr_branchless_clip(d_freq, d_max_freq);
  
#if VERBOSE_COSTAS
  printf("cl: phase_error: %f  phase: %f  freq: %f  sample: %f+j%f  constellation: %f+j%f\n",
	 phase_error, d_phase, d_freq, sample.real(), sample.imag(), 
	 d_constellation.constellation()[d_current_const_point].real(), d_constellation.constellation()[d_current_const_point].imag());
#endif
}

int
gr_constellation_receiver_cb::general_work (int noutput_items,
					    gr_vector_int &ninput_items,
					    gr_vector_const_void_star &input_items,
					    gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];

  int i=0, o=0;

  float phase_error;
  unsigned int sym_value;
  gr_complex sample;

  while((o < noutput_items) && (i < ninput_items[0])) {
    sample = in[i];
    sym_value = d_constellation.decision_maker(sample);
    //std::cout << "sym_value: " << sym_value << " sample: " << sample << std::endl;
    phase_error = -arg(sample*conj(d_constellation.constellation()[sym_value]));
    phase_error_tracking(phase_error);  // corrects phase and frequency offsets
    out[o++] = sym_value;
    i++;
  }

  #if 0
  printf("ninput_items: %d   noutput_items: %d   consuming: %d   returning: %d\n",
	 ninput_items[0], noutput_items, i, o);
  #endif

  consume_each(i);
  return o;
}

// Base Constellation Class

gr_constellation::gr_constellation (std::vector<gr_complex> constellation) {
  d_constellation = constellation;
}

// Chooses points base on shortest distance.
// Inefficient.
unsigned int gr_constellation::decision_maker(gr_complex sample)
{
  unsigned int table_size = constellation().size();
  unsigned int min_index = 0;
  float min_euclid_dist;
  float euclid_dist;
    
  min_euclid_dist = norm(sample - constellation()[0]); 
  min_index = 0; 
  for (unsigned int j = 1; j < table_size; j++){
    euclid_dist = norm(sample - constellation()[j]);
    if (euclid_dist < min_euclid_dist){
      min_euclid_dist = euclid_dist;
      min_index = j;
    }
  }
  return min_index;
}
