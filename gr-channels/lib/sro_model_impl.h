/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CHANNELS_SRO_MODEL_CC_IMPL_H
#define	INCLUDED_CHANNELS_SRO_MODEL_CC_IMPL_H

#include <gnuradio/filter/fractional_interpolator_cc.h>
#include <gnuradio/filter/mmse_fir_interpolator_cc.h>
#include <gnuradio/analog/fastnoise_source_f.h>
#include <gnuradio/channels/sro_model.h>

namespace gr {
  namespace channels {

    class CHANNELS_API sro_model_impl
      : public sro_model
    {
    private:
      float d_mu;
      float d_mu_inc;
      float d_sro;
      float d_samp_rate;
      float d_max_dev_hz;
      float d_std_dev_hz;
      gr::filter::mmse_fir_interpolator_cc *d_interp;
      gr::analog::fastnoise_source_f::sptr d_noise;
      double d_noise_seed;

    public:
      sro_model_impl(
                double sample_rate_hz,
                double std_dev_hz,
                double max_dev_hz,
                double noise_seed=0);
      ~sro_model_impl();

      void forecast(int noutput_items,
		    gr_vector_int &ninput_items_required);
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

      float mu() const;
      float interp_ratio() const;
      void set_mu(float mu);
      void set_interp_ratio(float interp_ratio);
      void setup_rpc();

      void set_std_dev(double _dev){ d_std_dev_hz = _dev; d_noise = gr::analog::fastnoise_source_f::make(analog::GR_GAUSSIAN, _dev, d_noise_seed); }
      void set_max_dev(double _dev){ d_max_dev_hz = _dev; }
      void set_samp_rate(double _rate){ d_samp_rate = _rate; }

      double std_dev() const { return d_std_dev_hz; }
      double max_dev() const { return d_max_dev_hz; }
      double samp_rate() const { return d_samp_rate; }

    };

  } /* namespace filter */
} /* namespace gr */

#endif
