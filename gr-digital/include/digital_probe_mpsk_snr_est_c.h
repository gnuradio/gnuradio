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
#ifndef INCLUDED_DIGITAL_PROBE_MPSK_SNR_EST_C_H
#define INCLUDED_DIGITAL_PROBE_MPSK_SNR_EST_C_H

#include <digital_api.h>
#include <gr_sync_block.h>
#include <digital_impl_mpsk_snr_est.h>

class digital_probe_mpsk_snr_est_c;
typedef boost::shared_ptr<digital_probe_mpsk_snr_est_c> digital_probe_mpsk_snr_est_c_sptr;

DIGITAL_API digital_probe_mpsk_snr_est_c_sptr
digital_make_probe_mpsk_snr_est_c(snr_est_type_t type, double alpha);

/*!
 * Provides various methods to compute SNR.
 */
class DIGITAL_API digital_probe_mpsk_snr_est_c : public gr_sync_block
{
 private:
  snr_est_type_t d_type;
  double d_alpha;
  digital_impl_mpsk_snr_est *d_snr_est;

  // Factory function returning shared pointer of this class
  friend DIGITAL_API digital_probe_mpsk_snr_est_c_sptr
    digital_make_probe_mpsk_snr_est_c(snr_est_type_t type, double alpha);
  
  // Private constructor
  digital_probe_mpsk_snr_est_c(snr_est_type_t type, double alpha);

public:

  ~digital_probe_mpsk_snr_est_c();

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

#endif /* INCLUDED_DIGITAL_PROBE_MPSK_SNR_EST_C_H */
