/* -*- c++ -*- */
/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_DD_MPSK_SYNC_CC_H
#define INCLUDED_GR_DD_MPSK_SYNC_CC_H

#include <gr_sync_block.h>

class gri_mmse_fir_interpolator_cc;

class gr_dd_mpsk_sync_cc;
typedef boost::shared_ptr<gr_dd_mpsk_sync_cc> gr_dd_mpsk_sync_cc_sptr;

gr_dd_mpsk_sync_cc_sptr 
gr_make_dd_mpsk_sync_cc (float alpha, float beta,
			 float max_freq, float min_freq, float ref_phase,
			 float omega, float gain_omega, float mu, float gain_mu);

/*!
 * \brief Decision directed M-PSK synchronous demod 
 * \ingroup clock
 * This block performs joint carrier tracking and symbol timing recovery.
 *
 * input: complex baseband; output: properly timed complex samples ready for slicing.
 *
 * N.B, at this point, it handles only QPSK.
 */

class gr_dd_mpsk_sync_cc : public gr_block
{
  friend gr_dd_mpsk_sync_cc_sptr gr_make_dd_mpsk_sync_cc (float alpha, float beta,
							  float max_freq, float min_freq, float ref_phase,
							  float omega, float gain_omega, float mu, float gain_mu);
public:
  ~gr_dd_mpsk_sync_cc ();
  void forecast(int noutput_items, gr_vector_int &ninput_items_required);
  float mu() const { return d_mu;}
  float omega() const { return d_omega;}
  float gain_mu() const { return d_gain_mu;}
  float gain_omega() const { return d_gain_omega;}
  
  void set_gain_mu (float gain_mu) { d_gain_mu = gain_mu; }
  void set_gain_omega (float gain_omega) { d_gain_omega = gain_omega; }
  void set_mu (float mu) { d_mu = mu; }
  void set_omega (float omega) { d_omega = omega; }

protected:
  gr_dd_mpsk_sync_cc (float alpha, float beta, float max_freq, float min_freq, float ref_phase,
		      float omega, float gain_omega, float mu, float gain_mu);
  
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
  
private:
  static const unsigned int DLLEN = 8;	// delay line length.

  float d_alpha,d_beta,d_max_freq,d_min_freq,d_ref_phase;
  float d_omega, d_gain_omega, d_mu, d_gain_mu;
  float d_phase, d_freq;
  gr_complex slicer_45deg (gr_complex sample);
  gr_complex slicer_0deg (gr_complex sample);
  gr_complex d_last_sample;
  gri_mmse_fir_interpolator_cc 	*d_interp;
  
  gr_complex d_dl[2 * DLLEN];	// Holds post carrier tracking samples.
                                // double length delay line to avoid wraps.
  unsigned int d_dl_idx;	// indexes oldest sample in delay line.

  float phase_detector(gr_complex sample,float ref_phase);
};

#endif
