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

#ifndef INCLUDED_CHANNELS_DYNAMIC_CHANNEL_MODEL_H
#define INCLUDED_CHANNELS_DYNAMIC_CHANNEL_MODEL_H

#include <gnuradio/channels/api.h>
#include <gnuradio/hier_block2.h>
#include <gnuradio/types.h>

namespace gr {
  namespace channels {

    /*!
     * \brief dynamic channel simulator
     * \ingroup dynamic_channel_models_blk
     *
     * \details
     * This block implements a dynamic channel model simulator that can
     * be used to help evaluate, design, and test various signals,
     * waveforms, and algorithms.
     *
     * This model allows the user to set up an AWGN noise cource, a
     * random walk process to simulate carrier frequency drift, a random
     * walk process to simulate sample rate offset drive, and a frequency
     * selective fading channel response that is either Rayleigh or Ricean
     * for a user specified power delay profile.
     */
    class CHANNELS_API dynamic_channel_model : virtual public hier_block2
    {
    public:
      // gr::channels::dynamic_channel_model::sptr
      typedef boost::shared_ptr<dynamic_channel_model> sptr;

      /*! \brief Build the dynamic channel simulator.
       *
       * \param samp_rate   Input sample rate in Hz
       * \param sro_std_dev  sample rate drift process standard deviation per sample in Hz
       * \param sro_max_dev  maximum sample rate offset in Hz
       * \param cfo_std_dev  carrier frequnecy drift process standard deviation per sample in Hz
       * \param cfo_max_dev  maximum carrier frequency offset in Hz

       * \param N   number of sinusoids used in frequency selective fading simulation
       * \param doppler_freq    maximum doppler frequency used in fading simulation in Hz
       * \param LOS_model   defines whether the fading model should include a line of site
                            component. LOS->Rician, NLOS->Rayleigh
       * \param K Rician K-factor, the ratio of specular to diffuse power in the model
       * \param delays  A list of fractional sample delays making up the power delay profile
       * \param mags    A list of magnitudes corresponding to each delay time in the power delay profile
       * \param ntaps_mpath The length of the filter to interpolate the power delay profile over.
                            Delays in the PDP must lie between 0 and ntaps_mpath, fractional delays
                            will be sinc-interpolated only to the width of this filter.
       * \param noise_amp Specifies the standard deviation of the AWGN process
       * \param noise_seed A random number generator seed for the noise source.
       */
      static sptr make(
                                            double samp_rate,
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
                                            double noise_seed );

      virtual double samp_rate() const = 0;
      virtual double sro_dev_std() const = 0;
      virtual double sro_dev_max() const = 0;
      virtual double cfo_dev_std() const = 0;
      virtual double cfo_dev_max() const = 0;
      virtual double noise_amp() const = 0;
      virtual double doppler_freq() const = 0;
      virtual double K() const = 0;

      virtual void set_samp_rate(double) = 0;
      virtual void set_sro_dev_std(double) = 0;
      virtual void set_sro_dev_max(double) = 0;
      virtual void set_cfo_dev_std(double) = 0;
      virtual void set_cfo_dev_max(double) = 0;
      virtual void set_noise_amp(double) = 0;
      virtual void set_doppler_freq(double) = 0;
      virtual void set_K(double) = 0;

    };

  } /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CHANNEL_MODEL_H */
