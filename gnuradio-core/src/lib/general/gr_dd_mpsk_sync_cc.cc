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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_dd_mpsk_sync_cc.h>
#include <gr_io_signature.h>
#include <gr_sincos.h>
#include <gri_mmse_fir_interpolator_cc.h>
#include <math.h>
#include <stdexcept>

#include <gr_complex.h>

#define M_TWOPI (2*M_PI)

gr_dd_mpsk_sync_cc_sptr
gr_make_dd_mpsk_sync_cc (float alpha, float beta, float max_freq, float min_freq, float ref_phase,
			 float omega, float gain_omega, float mu, float gain_mu)
{
    return gr_dd_mpsk_sync_cc_sptr (new gr_dd_mpsk_sync_cc (alpha, beta, max_freq, min_freq,ref_phase,
							    omega,gain_omega,mu,gain_mu));
}

gr_dd_mpsk_sync_cc::gr_dd_mpsk_sync_cc (float alpha, float beta, float max_freq, float min_freq,
					float ref_phase,
					float omega, float gain_omega, float mu, float gain_mu)
    : gr_block ("dd_mpsk_sync_cc",
		gr_make_io_signature (1, 1, sizeof (gr_complex)),
		gr_make_io_signature (1, 1, sizeof (gr_complex))),
      d_alpha(alpha), d_beta(beta), 
      d_max_freq(max_freq), d_min_freq(min_freq),
      d_ref_phase(ref_phase),d_omega(omega), d_gain_omega(gain_omega), 
      d_mu(mu), d_gain_mu(gain_mu),
      d_phase(0), d_freq((max_freq+min_freq)/2), d_last_sample(0), 
      d_interp(new gri_mmse_fir_interpolator_cc()),
      d_dl_idx(0)
{
    if (omega <= 0.0)
	throw std::out_of_range ("clock rate must be > 0");
    if (gain_mu <  0  || gain_omega < 0)
	throw std::out_of_range ("Gains must be non-negative");

    assert(d_interp->ntaps() <= DLLEN);

    // zero double length delay line.
    for (unsigned int i = 0; i < 2 * DLLEN; i++)
      d_dl[i] = gr_complex(0.0,0.0);
}

gr_dd_mpsk_sync_cc::~gr_dd_mpsk_sync_cc()
{
    delete d_interp;
}

float
gr_dd_mpsk_sync_cc::phase_detector(gr_complex sample,float ref_phase)
{
  return ((sample.real()>0 ? 1.0 : -1.0) * sample.imag() -
	  (sample.imag()>0 ? 1.0 : -1.0) * sample.real());
}

void
gr_dd_mpsk_sync_cc::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size();
  for (unsigned i=0; i < ninputs; i++)
    ninput_items_required[i] =
      (int) ceil((noutput_items * d_omega) + d_interp->ntaps());
}
gr_complex
gr_dd_mpsk_sync_cc::slicer_45deg (gr_complex sample)
{
  float real,imag;
  if(sample.real() > 0)
    real=1;
  else
    real=-1;
  if(sample.imag() > 0)
    imag = 1;
  else
    imag = -1;
  return gr_complex(real,imag);
}

gr_complex
gr_dd_mpsk_sync_cc::slicer_0deg (gr_complex sample)
{
  gr_complex out;
  if( fabs(sample.real()) > fabs(sample.imag()) ) {
    if(sample.real() > 0)
      return gr_complex(1.0,0.0);
    else
      return gr_complex(-1.0,0.0);
  }
  else {
    if(sample.imag() > 0)
      return gr_complex(0.0, 1.0);
    else
      return gr_complex(0.0, -1.0);
  }
}

int
gr_dd_mpsk_sync_cc::general_work (int noutput_items, 
				  gr_vector_int &ninput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  const gr_complex *in = (gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];
  
  int ii, oo;
  ii = 0; oo = 0;
  
  float error;
  float t_imag, t_real;
  gr_complex nco_out;
  float mm_val;

  while (oo < noutput_items) {
    // 
    // generate an output sample by interpolating between the carrier
    // tracked samples in the delay line.  d_mu, the fractional
    // interpolation amount (in [0.0, 1.0]) is controlled by the
    // symbol timing loop below.
    //
    out[oo] = d_interp->interpolate (&d_dl[d_dl_idx], d_mu);

    error = phase_detector(out[oo], d_ref_phase);
    
    d_freq = d_freq + d_beta * error;
    d_phase = d_phase + d_alpha * error;
    while(d_phase>M_TWOPI)
      d_phase -= M_TWOPI;
    while(d_phase<-M_TWOPI)
      d_phase += M_TWOPI;
      
    if (d_freq > d_max_freq)
      d_freq = d_max_freq;
    else if (d_freq < d_min_freq)
      d_freq = d_min_freq;
      
    mm_val = real(d_last_sample * slicer_0deg(out[oo]) - out[oo] * slicer_0deg(d_last_sample));
    d_last_sample = out[oo];

    d_omega = d_omega + d_gain_omega * mm_val;
    d_mu = d_mu + d_omega + d_gain_mu * mm_val;
    
    while(d_mu >= 1.0) {
      //
      // Generate more carrier tracked samples for the delay line
      //
      d_mu -= 1.0;
      gr_sincosf(d_phase, &t_imag, &t_real);
      nco_out = gr_complex(t_real, -t_imag);
      gr_complex new_sample = in[ii] * nco_out;

      d_dl[d_dl_idx] = new_sample;		// overwrite oldest sample
      d_dl[(d_dl_idx + DLLEN)] = new_sample;	// and second copy
      d_dl_idx = (d_dl_idx+1) % DLLEN;		// point to the new oldest sample
      d_phase = d_phase + d_freq;
      ii++;
    }
    oo++;
    printf("%f\t%f\t%f\t%f\t%f\n",d_mu,d_omega,mm_val,d_freq,d_phase);
    //printf("%f\t%f\t%f\t%f\t%f\t%f\t%f\n",mple).real(),slicer_0deg(d_last_sample).imag(),mm_val,d_omega,d_mu);
  }

  assert(ii <= ninput_items[0]);

  consume_each (ii);
  return noutput_items;
}
