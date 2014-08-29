/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_PROBE_MPSK_SNR_EST_C_IMPL_H
#define INCLUDED_DIGITAL_PROBE_MPSK_SNR_EST_C_IMPL_H

#include <gnuradio/digital/probe_mpsk_snr_est_c.h>

namespace gr {
  namespace digital {

    class probe_mpsk_snr_est_c_impl : public probe_mpsk_snr_est_c
    {
    private:
      snr_est_type_t d_type;
      int d_nsamples, d_count;
      double d_alpha;
      mpsk_snr_est *d_snr_est;

      // Message port names
      pmt::pmt_t d_snr_port;
      pmt::pmt_t d_signal_port;
      pmt::pmt_t d_noise_port;

    public:
      probe_mpsk_snr_est_c_impl(snr_est_type_t type,
				int msg_nsamples=10000,
				double alpha=0.001);

      ~probe_mpsk_snr_est_c_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      //! Return the estimated signal-to-noise ratio in decibels
      double snr();

      //! Return the estimated signal power in decibels
      double signal();

      //! Return the estimated noise power in decibels
      double noise();

      //! Return the type of estimator in use
      snr_est_type_t type() const;

      //! Return how many samples between SNR messages
      int msg_nsample() const;

      //! Get the running-average coefficient
      double alpha() const;

      //! Set type of estimator to use
      void set_type(snr_est_type_t t);

      //! Set the number of samples between SNR messages
      void set_msg_nsample(int n);

      //! Set the running-average coefficient
      void set_alpha(double alpha);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_PROBE_MPSK_SNR_EST_C_IMPL_H */
