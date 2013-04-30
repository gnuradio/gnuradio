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

#ifndef INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H
#define INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H

#include <gnuradio/sync_block.h>
#include <gnuradio/channels/fading_model.h>

//#include <iostream>
#include <boost/format.hpp>
#include <boost/random.hpp>

#include <gnuradio/fxpt.h>
#include <sincostable.h>

namespace gr {
  namespace channels {

    class CHANNELS_API fading_model_impl : public fading_model
    {
    private:
        // initial theta variate generator
        boost::mt19937 seed_1;
        boost::uniform_real<> dist_1; // U(-pi,pi)
        boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rv_1;

        // random walk variate
        boost::mt19937 seed_2;
        boost::uniform_real<> dist_2; // U(-pi,pi)
        boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rv_2;

        int d_N; // number of sinusoids
        float d_fDTs;  // normalized maximum doppler frequency
        double d_theta; // random walk variable (RWP)
        float d_theta_los;
        float d_step;  // maximum random walk step size
        uint64_t d_m;  // sample counter

        float d_K;     // Rician factor (ratio of the specular power to the scattered power)
        bool d_LOS;    // LOS path exists? chooses Rician (LOS) vs Rayleigh (NLOS) model.

        std::vector<float> d_psi; // in-phase initial phase
        std::vector<float> d_phi; // quadrature initial phase

        std::vector<float>  d_costable;

        sincostable d_table;

        float scale_sin, scale_los, scale_nlos; 

        void update_theta();

    public:
      fading_model_impl(unsigned int N, float fDTs, bool LOS, float K, int seed);
      ~fading_model_impl();
      void setup_rpc();
      int work (int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);

      virtual float fDTs(){ return d_fDTs; }
      virtual float K(){ return d_K; }
      virtual float step(){ return d_step; }

      virtual void set_fDTs(float fDTs){ d_fDTs = fDTs;  d_step = powf(0.00125*fDTs, 1.1); }
      virtual void set_K(float K){ d_K = K; scale_los = sqrtf(d_K)/sqrtf(d_K+1); scale_nlos = (1/sqrtf(d_K+1)); }
      virtual void set_step(float step){ d_step = step; }

    };

  } /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H */
