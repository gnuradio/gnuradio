/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_IMPL_H
#define	INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_IMPL_H

#include <digital/clock_recovery_mm_ff.h>
#include <filter/mmse_fir_interpolator_ff.h>

namespace gr {
  namespace digital {

    class clock_recovery_mm_ff_impl : public clock_recovery_mm_ff
    {
    public:
      clock_recovery_mm_ff_impl(float omega, float gain_omega,
				float mu, float gain_mu,
				float omega_relative_limi);
      ~clock_recovery_mm_ff_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

      float mu() const { return d_mu;}
      float omega() const { return d_omega;}
      float gain_mu() const { return d_gain_mu;}
      float gain_omega() const { return d_gain_omega;}

      void set_verbose (bool verbose) { d_verbose = verbose; }
      void set_gain_mu (float gain_mu) { d_gain_mu = gain_mu; }
      void set_gain_omega (float gain_omega) { d_gain_omega = gain_omega; }
      void set_mu (float mu) { d_mu = mu; }
      void set_omega (float omega){
	d_omega = omega;
	d_min_omega = omega*(1.0 - d_omega_relative_limit);
	d_max_omega = omega*(1.0 + d_omega_relative_limit);
	d_omega_mid = 0.5*(d_min_omega+d_max_omega);
      }

    private:
      float d_mu;                   // fractional sample position [0.0, 1.0]
      float d_gain_mu;              // gain for adjusting mu
      float d_omega;                // nominal frequency
      float d_gain_omega;           // gain for adjusting omega
      float d_min_omega;	    // minimum allowed omega
      float d_max_omega;	    // maximum allowed omeg
      float d_omega_relative_limit; // used to compute min and max omega
      float d_omega_mid;            // average omega

      float d_last_sample;
      filter::mmse_fir_interpolator_ff *d_interp;

      bool d_verbose;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_IMPL_H */
