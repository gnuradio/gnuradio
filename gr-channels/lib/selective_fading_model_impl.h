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

#ifndef INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL_IMPL_H
#define INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL_IMPL_H

#include <gnuradio/sync_block.h>
#include <gnuradio/channels/selective_fading_model.h>
#include "flat_fader_impl.h"

//#include <iostream>
#include <boost/format.hpp>
#include <boost/random.hpp>

#include <gnuradio/fxpt.h>
#include <sincostable.h>

namespace gr {
  namespace channels {

    class CHANNELS_API selective_fading_model_impl : public selective_fading_model
    {
    private:
      std::vector<gr::channels::flat_fader_impl*> d_faders;
      std::vector<float> d_delays;
      std::vector<float> d_mags;
      sincostable d_sintable;

    public:
      selective_fading_model_impl(unsigned int N, float fDTs, bool LOS, float K, int seed, std::vector<float> delays, std::vector<float> mags, int ntaps);
      ~selective_fading_model_impl();
      void setup_rpc();
      int work (int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
      std::vector<gr_complex> d_taps;

      virtual float fDTs(){ return d_faders[0]->d_fDTs; }
      virtual float K(){ return d_faders[0]->d_K; }
      virtual float step(){ return d_faders[0]->d_step; }

      virtual void set_fDTs(float fDTs){
            BOOST_FOREACH( gr::channels::flat_fader_impl* fader, d_faders )
            { fader->d_fDTs = fDTs;  fader->d_step = powf(0.00125*fDTs, 1.1); }
            }
      virtual void set_K(float K){
            BOOST_FOREACH( gr::channels::flat_fader_impl* fader, d_faders )
            { fader->d_K = K; fader->scale_los = sqrtf(fader->d_K)/sqrtf(fader->d_K+1); fader->scale_nlos = (1/sqrtf(fader->d_K+1)); }
            }
      virtual void set_step(float step){
            BOOST_FOREACH( gr::channels::flat_fader_impl* fader, d_faders )
            { fader->d_step = step; }
            }

    };

  } /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL_IMPL_H */
