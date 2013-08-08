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
#include "flat_fader_impl.h"

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
      gr::channels::flat_fader_impl d_fader;

    public:
      fading_model_impl(unsigned int N, float fDTs, bool LOS, float K, int seed);
      ~fading_model_impl();
      void setup_rpc();
      int work (int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);

      virtual float fDTs(){ return d_fader.d_fDTs; }
      virtual float K(){ return d_fader.d_K; }
      virtual float step(){ return d_fader.d_step; }

      virtual void set_fDTs(float fDTs){ d_fader.d_fDTs = fDTs;  d_fader.d_step = powf(0.00125*fDTs, 1.1); }
      virtual void set_K(float K){ d_fader.d_K = K; d_fader.scale_los = sqrtf(d_fader.d_K)/sqrtf(d_fader.d_K+1); d_fader.scale_nlos = (1/sqrtf(d_fader.d_K+1)); }
      virtual void set_step(float step){ d_fader.d_step = step; }

    };

  } /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H */
