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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "probe_mpsk_snr_est_c_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>

namespace gr {
  namespace digital {

    probe_mpsk_snr_est_c::sptr
    probe_mpsk_snr_est_c::make(snr_est_type_t type,
			       int msg_nsamples,
			       double alpha)
    {
      return gnuradio::get_initial_sptr
	(new probe_mpsk_snr_est_c_impl(type, msg_nsamples, alpha));
    }

    probe_mpsk_snr_est_c_impl::probe_mpsk_snr_est_c_impl(snr_est_type_t type,
							 int msg_nsamples,
							 double alpha)
      : sync_block("probe_mpsk_snr_est_c",
		      io_signature::make(1, 1, sizeof(gr_complex)),
		      io_signature::make(0, 0, 0))
    {
      d_snr_est = NULL;

      d_type = type;
      d_nsamples = msg_nsamples;
      d_count = 0;
      set_alpha(alpha);

      set_type(type);

      // at least 1 estimator has to look back
      set_history(2);

      d_snr_port = pmt::string_to_symbol("snr");
      d_signal_port = pmt::string_to_symbol("signal");
      d_noise_port = pmt::string_to_symbol("noise");

      message_port_register_out(d_snr_port);
      message_port_register_out(d_signal_port);
      message_port_register_out(d_noise_port);
    }

    probe_mpsk_snr_est_c_impl::~probe_mpsk_snr_est_c_impl()
    {
      if(d_snr_est)
	delete d_snr_est;
    }

    int
    probe_mpsk_snr_est_c_impl::work(int noutput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      int n = d_snr_est->update(noutput_items, in);

      d_count += noutput_items;
      while(d_count > d_nsamples) {
	// Post a message with the latest SNR data
	message_port_pub(d_snr_port, pmt::from_double(snr()));
	message_port_pub(d_signal_port, pmt::from_double(signal()));
	message_port_pub(d_noise_port, pmt::from_double(noise()));
	d_count -= d_nsamples;
      }

      return n;
    }

    double
    probe_mpsk_snr_est_c_impl::snr()
    {
      if(d_snr_est)
	return d_snr_est->snr();
      else
	throw std::runtime_error("probe_mpsk_snr_est_c_impl:: No SNR estimator defined.\n");
    }

    double
    probe_mpsk_snr_est_c_impl::signal()
    {
      if(d_snr_est)
	return d_snr_est->signal();
      else
	throw std::runtime_error("probe_mpsk_snr_est_c_impl:: No SNR estimator defined.\n");
    }


    double
    probe_mpsk_snr_est_c_impl::noise()
    {
      if(d_snr_est)
	return d_snr_est->noise();
      else
	throw std::runtime_error("probe_mpsk_snr_est_c_impl:: No SNR estimator defined.\n");
    }

    snr_est_type_t
    probe_mpsk_snr_est_c_impl::type() const
    {
      return d_type;
    }

    int
    probe_mpsk_snr_est_c_impl::msg_nsample() const
    {
      return d_nsamples;
    }

    double
    probe_mpsk_snr_est_c_impl::alpha() const
    {
      return d_alpha;
    }

    void
    probe_mpsk_snr_est_c_impl::set_type(snr_est_type_t t)
    {
      d_type = t;

      if(d_snr_est)
	delete d_snr_est;

      switch (d_type) {
      case(SNR_EST_SIMPLE):
	d_snr_est = new mpsk_snr_est_simple(d_alpha);
	break;
      case(SNR_EST_SKEW):
	d_snr_est = new mpsk_snr_est_skew(d_alpha);
	break;
      case(SNR_EST_M2M4):
	d_snr_est = new mpsk_snr_est_m2m4(d_alpha);
	break;
      case(SNR_EST_SVR):
	d_snr_est = new mpsk_snr_est_svr(d_alpha);
	break;
      default:
	throw std::invalid_argument("probe_mpsk_snr_est_c_impl: unknown type specified.\n");
      }
    }

    void
    probe_mpsk_snr_est_c_impl::set_msg_nsample(int n)
    {
      if(n > 0) {
	d_nsamples = n;
	d_count = 0;    // reset state
      }
      else
	throw std::invalid_argument("probe_mpsk_snr_est_c_impl: msg_nsamples can't be <= 0\n");
    }

    void
    probe_mpsk_snr_est_c_impl::set_alpha(double alpha)
    {
      if((alpha >= 0) && (alpha <= 1.0)) {
	d_alpha = alpha;
	if(d_snr_est)
	  d_snr_est->set_alpha(d_alpha);
      }
      else
	throw std::invalid_argument("probe_mpsk_snr_est_c_impl: alpha must be in [0,1]\n");
    }

  } /* namespace digital */
} /* namespace gr */
