/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL_H
#define INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL_H

#include <gnuradio/channels/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/types.h>

namespace gr {
  namespace channels {
    
    const float default_delays[] =  {0.0,0.1,0.5};
    const float default_mags[] =    {1.0,0.8,0.3};

    /*!
     * \brief fading simulator
     * \ingroup channel_models_blk
     *
     * \details
     * This block implements a basic fading model simulator that can
     * be used to help evaluate, design, and test various signals,
     * waveforms, and algorithms. 
     */
    class CHANNELS_API selective_fading_model : virtual public sync_block
    {
    public:
      // gr::channels::channel_model::sptr
      typedef boost::shared_ptr<selective_fading_model> sptr;

      /*! \brief Build the channel simulator.
       *
       * \param N      The number of sinusiods to use in simulating the channel; 8 is a good value 
       * \param fDTs   normalized maximum Doppler frequency, fD * Ts
       * \param LOS    include Line-of-Site path? selects between Rayleigh (NLOS) and Rician (LOS) models
       * \param K      Rician factor (ratio of the specular power to the scattered power)
       * \param seed   a random number to seed the noise generators
       * \param delays A vector of values the specify the time delay of each impulse
       * \param mags   A vector of values that specifies the magnitude of each impulse
       * \param ntaps  The number of filter taps.
       */
      static sptr make(unsigned int N,
                float fDTs,
                bool LOS,
                float K,
                int seed,
                std::vector<float> delays,
                std::vector<float> mags,
                int ntaps
                );
      
      virtual float fDTs() = 0;
      virtual float K() = 0;
      virtual float step() = 0;

      virtual void set_fDTs(float fDTs) = 0;
      virtual void set_K(float K) = 0;
      virtual void set_step(float step) = 0;

    };

  } /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL_H */
