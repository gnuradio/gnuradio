/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H
#define INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H

#include <gnuradio/top_block.h>
#include <gnuradio/blocks/add_cc.h>
#include <gnuradio/blocks/multiply_cc.h>
#include <gnuradio/analog/sig_source_c.h>
#include <gnuradio/analog/fastnoise_source_c.h>
#include <gnuradio/channels/dynamic_channel_model.h>
#include <gnuradio/channels/selective_fading_model.h>
#include <gnuradio/channels/sro_model.h>
#include <gnuradio/channels/cfo_model.h>
#include <gnuradio/filter/fractional_resampler_cc.h>
#include <gnuradio/filter/fir_filter_ccc.h>

namespace gr {
  namespace channels {

    class CHANNELS_API dynamic_channel_model_impl : public dynamic_channel_model
    {
    private:

      channels::sro_model::sptr d_sro_model;
      channels::cfo_model::sptr d_cfo_model;
      channels::selective_fading_model::sptr d_fader;
      blocks::add_cc::sptr d_noise_adder;
      analog::fastnoise_source_c::sptr d_noise;

    public:
      dynamic_channel_model_impl( double samp_rate,
                                            double sro_std_dev,
                                            double sro_max_dev,
                                            double cfo_std_dev,
                                            double cfo_max_dev,
                                            unsigned int N,
                                            double doppler_freq,
                                            bool LOS_model,
                                            float K,
                                            std::vector<float> delays,
                                            std::vector<float> mags,
                                            int ntaps_mpath,
                                            double noise_amp,
                                            double noise_seed);

      ~dynamic_channel_model_impl();

      void setup_rpc();

      double samp_rate() const   { return d_sro_model->samp_rate(); }
      double sro_dev_std() const { return d_sro_model->std_dev(); }
      double sro_dev_max() const { return d_sro_model->max_dev(); }
      double cfo_dev_std() const { return d_cfo_model->std_dev(); }
      double cfo_dev_max() const { return d_cfo_model->max_dev(); }
      double noise_amp() const   { return d_noise->amplitude();   }
      double doppler_freq() const { return d_fader->fDTs()*samp_rate(); }
      double K() const { return d_fader->K(); }

      void set_samp_rate(double sr) {
            d_fader->set_fDTs( doppler_freq()/samp_rate() );
            d_sro_model->set_samp_rate(sr);
            d_cfo_model->set_samp_rate(sr);  }
      void set_sro_dev_std(double dev) { d_sro_model->set_max_dev(dev); }
      void set_sro_dev_max(double dev) { d_sro_model->set_max_dev(dev); }
      void set_cfo_dev_std(double dev) { d_cfo_model->set_std_dev(dev); }
      void set_cfo_dev_max(double dev) { d_cfo_model->set_max_dev(dev); }
      void set_noise_amp(double amp) { d_noise->set_amplitude(amp); }
      void set_doppler_freq(double f){ d_fader->set_fDTs(f/samp_rate()); }
      void set_K(double K){ d_fader->set_K(K); }

    };

  } /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H */
