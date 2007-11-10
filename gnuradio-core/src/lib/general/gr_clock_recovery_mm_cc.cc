/* -*- c++ -*- */
/*
 * Copyright 2005,2006 Free Software Foundation, Inc.
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
#include <gr_clock_recovery_mm_cc.h>
#include <gri_mmse_fir_interpolator_cc.h>
#include <stdexcept>

// Public constructor


gr_clock_recovery_mm_cc_sptr 
gr_make_clock_recovery_mm_cc(float omega, float gain_omega, float mu, float gain_mu,
			     float omega_relative_limit)
{
  return gr_clock_recovery_mm_cc_sptr (new gr_clock_recovery_mm_cc (omega, 
								    gain_omega, 
								    mu,
								    gain_mu,
								    omega_relative_limit));
}

gr_clock_recovery_mm_cc::gr_clock_recovery_mm_cc (float omega, float gain_omega, float mu, 
						  float gain_mu, float omega_relative_limit)
  : gr_block ("clock_recovery_mm_cc",
	      gr_make_io_signature (1, 1, sizeof (gr_complex)),
	      gr_make_io_signature (1, 2, sizeof (gr_complex))),
    d_mu (mu), d_omega(omega), d_gain_omega(gain_omega), 
    d_omega_relative_limit(omega_relative_limit), 
    d_gain_mu(gain_mu), d_last_sample(0), d_interp(new gri_mmse_fir_interpolator_cc()),
    d_verbose(gr_prefs::singleton()->get_bool("clock_recovery_mm_cc", "verbose", false)),
    d_p_2T(0), d_p_1T(0), d_p_0T(0), d_c_2T(0), d_c_1T(0), d_c_0T(0)
{
  if (omega <= 0.0)
    throw std::out_of_range ("clock rate must be > 0");
  if (gain_mu <  0  || gain_omega < 0)
    throw std::out_of_range ("Gains must be non-negative");

  set_omega(omega);			// also sets min and max omega
  set_relative_rate (1.0 / omega);
  set_history(3);			// ensure 2 extra input sample is available
}

gr_clock_recovery_mm_cc::~gr_clock_recovery_mm_cc ()
{
  delete d_interp;
}

void
gr_clock_recovery_mm_cc::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size();
  for (unsigned i=0; i < ninputs; i++)
    ninput_items_required[i] =
      (int) ceil((noutput_items * d_omega) + d_interp->ntaps());
}

gr_complex
gr_clock_recovery_mm_cc::slicer_0deg (gr_complex sample)
{
  float real=0, imag=0;

  if(sample.real() > 0)
    real = 1;
  if(sample.imag() > 0)
    imag = 1;
  return gr_complex(real,imag);
}

gr_complex
gr_clock_recovery_mm_cc::slicer_45deg (gr_complex sample)
{
  float real= -1, imag = -1;
  if(sample.real() > 0)
    real=1;
  if(sample.imag() > 0)
    imag = 1;
  return gr_complex(real,imag);
}

/*
  Modified Mueller and Muller clock recovery circuit
  Based:
     G. R. Danesfahani, T.G. Jeans, "Optimisation of modified Mueller and Muller 
     algorithm,"  Electronics Letters, Vol. 31, no. 13,  22 June 1995, pp. 1032 - 1033.
*/

static const int FUDGE = 16;

int
gr_clock_recovery_mm_cc::general_work (int noutput_items,
				       gr_vector_int &ninput_items,
				       gr_vector_const_void_star &input_items,
				       gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];
  gr_complex *foptr = (gr_complex *) output_items[1];

  bool write_foptr = output_items.size() >= 2;
  
  int  ii = 0;				// input index
  int  oo = 0;				// output index
  int  ni = ninput_items[0] - d_interp->ntaps() - FUDGE;  // don't use more input than this

  assert(d_mu >= 0.0);
  assert(d_mu <= 1.0);

  float mm_val=0;
  gr_complex u, x, y;

  // This loop writes the error to the second output, if it exists
  if (write_foptr) {
    while(oo < noutput_items && ii < ni) {
      d_p_2T = d_p_1T;
      d_p_1T = d_p_0T;
      d_p_0T = d_interp->interpolate (&in[ii], d_mu);

      d_c_2T = d_c_1T;
      d_c_1T = d_c_0T;
      d_c_0T = slicer_0deg(d_p_0T);
      
      x = (d_c_0T - d_c_2T) * conj(d_p_1T);
      y = (d_p_0T - d_p_2T) * conj(d_c_1T);
      u = y - x;
      mm_val = u.real();
      out[oo++] = d_p_0T;
      
      // limit mm_val
      if (mm_val > 1.0)
	mm_val = 1.0;
      else if (mm_val < -1.0)
	mm_val = -1.0;
      
      d_omega = d_omega + d_gain_omega * mm_val;
      if (d_omega > d_max_omega)
	d_omega = d_max_omega;
      else if (d_omega < d_min_omega)
	d_omega = d_min_omega;
      
      d_mu = d_mu + d_omega + d_gain_mu * mm_val;
      ii += (int)floor(d_mu);
      d_mu -= floor(d_mu);
      
      #if 0
      printf("%f\t%f\n", d_omega, d_mu);
      #endif
      
      // write the error signal to the second output
      foptr[oo-1] = gr_complex(d_mu,0);
      
      if (ii < 0)	// clamp it.  This should only happen with bogus input
	ii = 0;
    }
  }
  // This loop does not write to the second output (ugly, but faster)
  else {
    while(oo < noutput_items && ii < ni) {
      d_p_2T = d_p_1T;
      d_p_1T = d_p_0T;
      d_p_0T = d_interp->interpolate (&in[ii], d_mu);

      d_c_2T = d_c_1T;
      d_c_1T = d_c_0T;
      d_c_0T = slicer_0deg(d_p_0T);
      
      x = (d_c_0T - d_c_2T) * conj(d_p_1T);
      y = (d_p_0T - d_p_2T) * conj(d_c_1T);
      u = y - x;
      mm_val = u.real();
      out[oo++] = d_p_0T;
      
      // limit mm_val
      if (mm_val > 1.0)
	mm_val = 1.0;
      else if (mm_val < -1.0)
	mm_val = -1.0;
      
      d_omega = d_omega + d_gain_omega * mm_val;
      if (d_omega > d_max_omega)
	d_omega = d_max_omega;
      else if (d_omega < d_min_omega)
	d_omega = d_min_omega;
      
      d_mu = d_mu + d_omega + d_gain_mu * mm_val;
      ii += (int)floor(d_mu);
      d_mu -= floor(d_mu);
      
      if(d_verbose) {
	printf("%f\t%f\n", d_omega, d_mu);
      }
            
      if (ii < 0)	// clamp it.  This should only happen with bogus input
	ii = 0;
    }
  }

  if (ii > 0){
    if (ii > ninput_items[0]){
      fprintf(stderr, "gr_clock_recovery_mm_cc: ii > ninput_items[0] (%d > %d)\n",
	      ii, ninput_items[0]);
      assert(0);
    }
    consume_each (ii);
  }

  return oo;
}
