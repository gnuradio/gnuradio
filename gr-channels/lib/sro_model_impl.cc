/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2010,2012 Free Software Foundation, Inc.
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

#include <gnuradio/io_signature.h>
#include "sro_model_impl.h"
#include <stdexcept>

namespace gr {
  namespace channels {

    sro_model::sptr
    sro_model::make(
                double sample_rate_hz,
                double std_dev_hz,
                double max_dev_hz,
                double noise_seed)
    {
      return gnuradio::get_initial_sptr(
          new sro_model_impl(sample_rate_hz, std_dev_hz, max_dev_hz, noise_seed));
    }

    sro_model_impl::sro_model_impl(
                double sample_rate_hz,
                double std_dev_hz,
                double max_dev_hz,
                double noise_seed)
      : block("sro_model",
		 io_signature::make(1, 1, sizeof(gr_complex)),
		 io_signature::make(1, 1, sizeof(gr_complex))),
	d_mu (0.0), d_mu_inc (1.0), d_sro(0.0), d_samp_rate(sample_rate_hz),
    d_max_dev_hz(max_dev_hz), d_std_dev_hz(std_dev_hz),
	d_interp(new gr::filter::mmse_fir_interpolator_cc()),
    d_noise(gr::analog::fastnoise_source_f::make(analog::GR_GAUSSIAN, std_dev_hz, noise_seed)),
    d_noise_seed(noise_seed)
    {
      //set_relative_rate(1.0 / interp_ratio);
      set_relative_rate(1.0);
    }

    sro_model_impl::~sro_model_impl()
    {
      delete d_interp;
    }

    void
    sro_model_impl::forecast(int noutput_items,
				     gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++) {
	ninput_items_required[i] =
	  (int)ceil((noutput_items * (d_mu_inc + d_max_dev_hz/d_samp_rate)) + d_interp->ntaps());
      }
    }

    int
    sro_model_impl::general_work(int noutput_items,
				     gr_vector_int &ninput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      int ii = 0; // input index
      int oo = 0; // output index

      while(oo < noutput_items) {

    // perform sample rate offset update
    d_sro += d_noise->sample_unbiased();
    d_sro = std::min(d_sro, d_max_dev_hz);
    d_sro = std::max(d_sro, -d_max_dev_hz);
    d_mu_inc = 1.0 + d_sro/d_samp_rate;

	out[oo++] = d_interp->interpolate(&in[ii], d_mu);

	double s = d_mu + d_mu_inc;
	double f = floor(s);
	int incr = (int)f;
	d_mu = s - f;
	ii += incr;
      }

      consume_each(ii);

      return noutput_items;
    }

    float
    sro_model_impl::mu() const
    {
      return d_mu;
    }

    float
    sro_model_impl::interp_ratio() const
    {
      return d_mu_inc;
    }

    void
    sro_model_impl::set_mu(float mu)
    {
      d_mu = mu;
    }

    void
    sro_model_impl::set_interp_ratio(float interp_ratio)
    {
      d_mu_inc = interp_ratio;
    }

    void
    sro_model_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_variable<float>(
      alias(), "sro", &d_sro,
      pmt::mp(-10.0f), pmt::mp(10.0f), pmt::mp(0.0f),
      "", "Current SRO in Hz", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTSTRIP)));
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_variable<float>(
      alias(), "sro_max", &d_max_dev_hz,
      pmt::mp(-10.0f), pmt::mp(10.0f), pmt::mp(0.0f),
      "", "Max SRO in Hz", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace filter */
} /* namespace gr */
