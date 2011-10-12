/* -*- c++ -*- */
/*
 * Copyright 2004,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_H
#define	INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_H

#include <digital_api.h>
#include <gr_block.h>
#include <gr_math.h>
#include <stdio.h>

class gri_mmse_fir_interpolator;

class digital_clock_recovery_mm_ff;
typedef boost::shared_ptr<digital_clock_recovery_mm_ff> digital_clock_recovery_mm_ff_sptr;

// public constructor
DIGITAL_API digital_clock_recovery_mm_ff_sptr 
digital_make_clock_recovery_mm_ff (float omega, float gain_omega,
				   float mu, float gain_mu,
				   float omega_relative_limit=0.001);

/*!
 * \brief Mueller and Müller (M&M) based clock recovery block with float input, float output.
 * \ingroup sync_blk
 * \ingroup digital
 *
 * This implements the Mueller and Müller (M&M) discrete-time error-tracking synchronizer.
 *
 * See "Digital Communication Receivers: Synchronization, Channel
 * Estimation and Signal Processing" by Heinrich Meyr, Marc Moeneclaey, & Stefan Fechtel.
 * ISBN 0-471-50275-8.
 */
class DIGITAL_API digital_clock_recovery_mm_ff : public gr_block
{
 public:
  ~digital_clock_recovery_mm_ff ();
  void forecast(int noutput_items, gr_vector_int &ninput_items_required);
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
  float mu() const { return d_mu;}
  float omega() const { return d_omega;}
  float gain_mu() const { return d_gain_mu;}
  float gain_omega() const { return d_gain_omega;}

  void set_gain_mu (float gain_mu) { d_gain_mu = gain_mu; }
  void set_gain_omega (float gain_omega) { d_gain_omega = gain_omega; }
  void set_mu (float mu) { d_mu = mu; }
  void set_omega (float omega){
    d_omega = omega;
    d_min_omega = omega*(1.0 - d_omega_relative_limit);
    d_max_omega = omega*(1.0 + d_omega_relative_limit);
    d_omega_mid = 0.5*(d_min_omega+d_max_omega);
  }

protected:
  digital_clock_recovery_mm_ff (float omega, float gain_omega, float mu, float gain_mu,
			   float omega_relative_limit);

 private:
  float 			d_mu;		// fractional sample position [0.0, 1.0]
  float 			d_omega;	// nominal frequency
  float				d_min_omega;	// minimum allowed omega 
  float                         d_omega_mid; 	// average omega
  float				d_max_omega;	// maximum allowed omega
  float                         d_gain_omega;	// gain for adjusting omega
  float                         d_gain_mu;	// gain for adjusting mu
  float                         d_last_sample;
  gri_mmse_fir_interpolator 	*d_interp;
  FILE				*d_logfile;
  float				d_omega_relative_limit;	// used to compute min and max omega

  friend DIGITAL_API digital_clock_recovery_mm_ff_sptr
  digital_make_clock_recovery_mm_ff (float omega, float gain_omega,
				     float mu, float gain_mu,
				     float omega_relative_limit);
};

#endif
