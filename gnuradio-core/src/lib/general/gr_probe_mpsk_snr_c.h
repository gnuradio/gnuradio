/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_PROBE_MPSK_SNR_C_H
#define INCLUDED_GR_PROBE_MPSK_SNR_C_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_probe_mpsk_snr_c;
typedef boost::shared_ptr<gr_probe_mpsk_snr_c> gr_probe_mpsk_snr_c_sptr;

GR_CORE_API gr_probe_mpsk_snr_c_sptr
gr_make_probe_mpsk_snr_c(double alpha = 0.0001);

/*!
 * Compute the estimate SNR of an MPSK signal using the Squared Signal
 * to Noise Variance (SNV) technique.
 *
 * This technique assumes an AWGN channel.
 *
 * \param alpha  Mean and variance smoothing filter constant
 * \ingroup sink_blk
 *
 * Compute the running average of the signal mean and noise variance.
 * The estimated signal mean, noise variance, and SNR are available
 * via accessors.
 *
 * This SNR estimator is inaccurate below about 7dB SNR.
 *
 */
class GR_CORE_API gr_probe_mpsk_snr_c : public gr_sync_block
{
  double d_alpha;
  double d_beta;
  double d_signal_mean;
  double d_noise_variance;

  // Factory function returning shared pointer of this class
  friend GR_CORE_API gr_probe_mpsk_snr_c_sptr
  gr_make_probe_mpsk_snr_c(double alpha);

  // Private constructor
  gr_probe_mpsk_snr_c(double alpha);

public:
  ~gr_probe_mpsk_snr_c();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  // Return the estimated signal mean
  double signal_mean() const { return d_signal_mean; }

  // Return the estimated noise variance
  double noise_variance() const { return d_noise_variance; }

  // Return the estimated signal-to-noise ratio in decibels
  double snr() const;

  void set_alpha(double alpha);
};

#endif /* INCLUDED_GR_PROBE_MPSK_SNR_C_H */
