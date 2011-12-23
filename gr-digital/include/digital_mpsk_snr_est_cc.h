/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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
#ifndef INCLUDED_DIGITAL_MPSK_SNR_EST_CC_H
#define INCLUDED_DIGITAL_MPSK_SNR_EST_CC_H

#include <digital_api.h>
#include <gr_sync_block.h>

class digital_mpsk_snr_est_cc;
typedef boost::shared_ptr<digital_mpsk_snr_est_cc> digital_mpsk_snr_est_cc_sptr;

enum snr_est_type_t {
  SNR_EST_SIMPLE = 0,	// Simple estimator (>= 7 dB)
  SNR_EST_SKEW,	        // Skewness-base est (>= 5 dB)
  SNR_EST_M2M4,         // 2nd & 4th moment est (>= 3 dB)
  SNR_EST_SVN           // SVN-based est (>= 0dB)
};

DIGITAL_API digital_mpsk_snr_est_cc_sptr
digital_make_mpsk_snr_est_cc(snr_est_type_t type, double alpha);

/*!
 * Provides various methods to compute SNR.
 */
class DIGITAL_API digital_mpsk_snr_est_cc : public gr_sync_block
{
 private:
  snr_est_type_t d_type;
  double d_y1, d_y2, d_y3, d_y4;
  double d_alpha, d_beta;

  // Function pointers to the type of estimator used.
  int (digital_mpsk_snr_est_cc::*d_estimator)(int noutput_items,
		 gr_vector_const_void_star &input_items,
		 gr_vector_void_star &output_items);
  double (digital_mpsk_snr_est_cc::*d_calculator)();

  // Factory function returning shared pointer of this class
  friend DIGITAL_API digital_mpsk_snr_est_cc_sptr
    digital_make_mpsk_snr_est_cc(snr_est_type_t type, double alpha);
  
  // Private constructor
  digital_mpsk_snr_est_cc(snr_est_type_t type, double alpha);

  int est_simple(int noutput_items,
		 gr_vector_const_void_star &input_items,
		 gr_vector_void_star &output_items);
  double est_simple_snr();

  int est_skew(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
  double est_skew_snr();

  int est_m2m4(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
  double est_m2m4_snr();

  int est_svn(int noutput_items,
	      gr_vector_const_void_star &input_items,
	      gr_vector_void_star &output_items);
  double est_svn_snr();
  
public:

  ~digital_mpsk_snr_est_cc();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  //! Return the estimated signal-to-noise ratio in decibels
  double snr();

  //! Return the type of estimator in use
  snr_est_type_t type() const;

  //! Get the running-average coefficient
  double alpha() const;

  //! Set type of estimator to use
  void set_type(snr_est_type_t t);

  //! Set the running-average coefficient
  void set_alpha(double alpha);
};

#endif /* INCLUDED_DIGITAL_MPSK_SNR_EST_CC_H */
