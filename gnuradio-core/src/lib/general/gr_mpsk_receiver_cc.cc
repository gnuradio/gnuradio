/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2007 Free Software Foundation, Inc.
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
#include <gr_mpsk_receiver_cc.h>
#include <stdexcept>
#include <gr_math.h>
#include <gr_expj.h>
#include <gri_mmse_fir_interpolator_cc.h>


#define M_TWOPI (2*M_PI)
#define VERBOSE_MM     0     // Used for debugging symbol timing loop
#define VERBOSE_COSTAS 0     // Used for debugging phase and frequency tracking

// Public constructor

gr_mpsk_receiver_cc_sptr 
gr_make_mpsk_receiver_cc(unsigned int M, float theta,
			 float alpha, float beta,
			 float fmin, float fmax,
			 float mu, float gain_mu, 
			 float omega, float gain_omega, float omega_rel)
{
  return gr_mpsk_receiver_cc_sptr (new gr_mpsk_receiver_cc (M, theta, 
							    alpha, beta,
							    fmin, fmax,
							    mu, gain_mu, 
							    omega, gain_omega, omega_rel));
}

gr_mpsk_receiver_cc::gr_mpsk_receiver_cc (unsigned int M, float theta, 
					  float alpha, float beta,
					  float fmin, float fmax,
					  float mu, float gain_mu, 
					  float omega, float gain_omega, float omega_rel)
  : gr_block ("mpsk_receiver_cc",
	      gr_make_io_signature (1, 1, sizeof (gr_complex)),
	      gr_make_io_signature (1, 1, sizeof (gr_complex))),
    d_M(M), d_theta(theta), 
    d_alpha(alpha), d_beta(beta), d_freq(0), d_max_freq(fmax), d_min_freq(fmin), d_phase(0),
    d_current_const_point(0),
    d_mu(mu), d_gain_mu(gain_mu), d_gain_omega(gain_omega), 
    d_omega_rel(omega_rel), d_max_omega(0), d_min_omega(0),
    d_p_2T(0), d_p_1T(0), d_p_0T(0), d_c_2T(0), d_c_1T(0), d_c_0T(0)
{
  d_interp = new gri_mmse_fir_interpolator_cc();
  d_dl_idx = 0;

  set_omega(omega);

  if (omega <= 0.0)
    throw std::out_of_range ("clock rate must be > 0");
  if (gain_mu <  0  || gain_omega < 0)
    throw std::out_of_range ("Gains must be non-negative");
  
  assert(d_interp->ntaps() <= DLLEN);
  
  // zero double length delay line.
  for (unsigned int i = 0; i < 2 * DLLEN; i++)
    d_dl[i] = gr_complex(0.0,0.0);

  // build the constellation vector from M
  make_constellation();
  
  // Select a phase detector and a decision maker for the modulation order
  switch(d_M) {
  case 2:  // optimized algorithms for BPSK
    d_phase_error_detector = &gr_mpsk_receiver_cc::phase_error_detector_generic; //bpsk;
    d_decision = &gr_mpsk_receiver_cc::decision_generic; //bpsk;
    break;

  case 4: // optimized algorithms for QPSK
    d_phase_error_detector = &gr_mpsk_receiver_cc::phase_error_detector_generic; //qpsk;
    d_decision = &gr_mpsk_receiver_cc::decision_generic; //qpsk;
    break;

  default: // generic algorithms for any M (power of 2?) but not pretty
    d_phase_error_detector = &gr_mpsk_receiver_cc::phase_error_detector_generic;
    d_decision = &gr_mpsk_receiver_cc::decision_generic;
    break;
  }

  set_history(3);			// ensure 2 extra input sample is available
}

gr_mpsk_receiver_cc::~gr_mpsk_receiver_cc ()
{
  delete d_interp;
}

void
gr_mpsk_receiver_cc::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size();
  for (unsigned i=0; i < ninputs; i++)
    ninput_items_required[i] = (int) ceil((noutput_items * d_omega) + d_interp->ntaps());
  //ninput_items_required[i] = (int)(d_omega);

}

// FIXME add these back in an test difference in performance
float
gr_mpsk_receiver_cc::phase_error_detector_qpsk(gr_complex sample) const
{
  float phase_error = ((sample.real()>0 ? 1.0 : -1.0) * sample.imag() -
		       (sample.imag()>0 ? 1.0 : -1.0) * sample.real());
  return -phase_error;
}

// FIXME add these back in an test difference in performance
float
gr_mpsk_receiver_cc::phase_error_detector_bpsk(gr_complex sample) const
{
  return (sample.real()*sample.imag());
}

float gr_mpsk_receiver_cc::phase_error_detector_generic(gr_complex sample) const
{
  //return gr_fast_atan2f(sample*conj(d_constellation[d_current_const_point]));
  return -arg(sample*conj(d_constellation[d_current_const_point]));
}

// FIXME add these back in an test difference in performance
unsigned int
gr_mpsk_receiver_cc::decision_bpsk(gr_complex sample) const
{
  unsigned int index = 0;

  // Implements a 1-demensional slicer
  if(sample.real() > 0)
    index = 1;
  return index;
}

// FIXME add these back in an test difference in performance
unsigned int
gr_mpsk_receiver_cc::decision_qpsk(gr_complex sample) const
{
  unsigned int index = 0;

  // Implements a simple slicer function
  if((sample.real() < 0) && (sample.imag() > 0))
    index = 1;
  else if((sample.real() < 0) && (sample.imag() < 0))
    index = 2;
  else
    index = 3;
  return index;
}

unsigned int
gr_mpsk_receiver_cc::decision_generic(gr_complex sample) const
{
  unsigned int min_m = 0;
  float min_s = 65535;
  
  // Develop all possible constellation points and find the one that minimizes
  // the Euclidean distance (error) with the sample
  for(unsigned int m=0; m < d_M; m++) {
    gr_complex diff = norm(d_constellation[m] - sample);
    
    if(fabs(diff.real()) < min_s) {
      min_s = fabs(diff.real());
      min_m = m;
    }
  }
  // Return the index of the constellation point that minimizes the error
  return min_m;
}


void
gr_mpsk_receiver_cc::make_constellation()
{
  for(unsigned int m=0; m < d_M; m++) {
    d_constellation.push_back(gr_expj((M_TWOPI/d_M)*m));
  }
}

void
gr_mpsk_receiver_cc::mm_sampler(const gr_complex symbol)
{
  gr_complex sample, nco;

  d_mu--;             // skip a number of symbols between sampling
  d_phase += d_freq;  // increment the phase based on the frequency of the rotation

  // Keep phase clamped and not walk to infinity
  while(d_phase>M_TWOPI)
    d_phase -= M_TWOPI;
  while(d_phase<-M_TWOPI)
    d_phase += M_TWOPI;

  nco = gr_expj(d_phase+d_theta);   // get the NCO value for derotating the current sample
  sample = nco*symbol;      // get the downconverted symbol
  
  // Fill up the delay line for the interpolator
  d_dl[d_dl_idx] = sample;
  d_dl[(d_dl_idx + DLLEN)] = sample;  // put this in the second half of the buffer for overflows
  d_dl_idx = (d_dl_idx+1) % DLLEN;    // Keep the delay line index in bounds
}

void
gr_mpsk_receiver_cc::mm_error_tracking(gr_complex sample)
{
  gr_complex u, x, y;
  float mm_error = 0;

  // Make sample timing corrections

  // set the delayed samples
  d_p_2T = d_p_1T;
  d_p_1T = d_p_0T;
  d_p_0T = sample;
  d_c_2T = d_c_1T;
  d_c_1T = d_c_0T;
  
  d_current_const_point = (*this.*d_decision)(d_p_0T);  // make a decision on the sample value
  d_c_0T = d_constellation[d_current_const_point];
  
  x = (d_c_0T - d_c_2T) * conj(d_p_1T);
  y = (d_p_0T - d_p_2T) * conj(d_c_1T);
  u = y - x;
  mm_error = u.real();   // the error signal is in the real part
  
  // limit mm_val
  if (mm_error > 1.0)
    mm_error = 1.0;
  else if (mm_error < -1.0)
    mm_error = -1.0;
  
  d_omega = d_omega + d_gain_omega * mm_error;  // update omega based on loop error

  // make sure we don't walk away
  if (d_omega > d_max_omega)
    d_omega = d_max_omega;
  else if (d_omega < d_min_omega)
    d_omega = d_min_omega;
  
  d_mu += d_omega + d_gain_mu * mm_error;   // update mu based on loop error
  
#if VERBOSE_MM
  printf("mm: mu: %f   omega: %f  mm_error: %f  sample: %f+j%f  constellation: %f+j%f\n", 
	 d_mu, d_omega, mm_error, sample.real(), sample.imag(), 
	 d_constellation[d_current_const_point].real(), d_constellation[d_current_const_point].imag());
#endif
}


void
gr_mpsk_receiver_cc::phase_error_tracking(gr_complex sample)
{
  float phase_error = 0;

  // Make phase and frequency corrections based on sampled value
  phase_error = (*this.*d_phase_error_detector)(sample);

  if (phase_error > 1)
    phase_error = 1;
  else if (phase_error < -1)
    phase_error = -1;  

  d_freq += d_beta*phase_error;             // adjust frequency based on error
  d_phase += d_freq + d_alpha*phase_error;  // adjust phase based on error
  
  // Make sure we stay within +-2pi
  while(d_phase>M_TWOPI)
    d_phase -= M_TWOPI;
  while(d_phase<-M_TWOPI)
    d_phase += M_TWOPI;
  
  // Limit the frequency range
  if (d_freq > d_max_freq)
    d_freq = d_max_freq;
  else if (d_freq < d_min_freq)
    d_freq = d_min_freq;

#if VERBOSE_COSTAS
  printf("cl: phase_error: %f  phase: %f  freq: %f  sample: %f+j%f  constellation: %f+j%f\n",
	 phase_error, d_phase, d_freq, sample.real(), sample.imag(), 
	 d_constellation[d_current_const_point].real(), d_constellation[d_current_const_point].imag());
#endif
}

int
gr_mpsk_receiver_cc::general_work (int noutput_items,
				   gr_vector_int &ninput_items,
				   gr_vector_const_void_star &input_items,
				   gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  int i=0, o=0;

  //while(i < ninput_items[0]) {    
  while((o < noutput_items) && (i < ninput_items[0])) {
    while((d_mu > 1) && (i < ninput_items[0]))  {
      mm_sampler(in[i]);   // puts symbols into a buffer and adjusts d_mu
      i++;
    }
    
    if(i < ninput_items[0]) {
      gr_complex interp_sample = d_interp->interpolate(&d_dl[d_dl_idx], d_mu);
       
      mm_error_tracking(interp_sample);     // corrects M&M sample time
      phase_error_tracking(interp_sample);  // corrects phase and frequency offsets

      out[o++] = interp_sample;
    }
  }

  #if 0
  printf("ninput_items: %d   noutput_items: %d   consuming: %d   returning: %d\n",
	 ninput_items[0], noutput_items, i, o);
  #endif

  consume_each(i);
  return o;
}
