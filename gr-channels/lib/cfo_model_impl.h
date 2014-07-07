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

#ifndef INCLUDED_CHANNELS_CFO_MODEL_IMPL_H
#define INCLUDED_CHANNELS_CFO_MODEL_IMPL_H

#include <gnuradio/top_block.h>
#include <gnuradio/blocks/integrate_ff.h>
#include <gnuradio/blocks/vco_c.h>
#include <gnuradio/blocks/multiply_cc.h>
#include <gnuradio/analog/fastnoise_source_f.h>
#include <gnuradio/channels/cfo_model.h>
#include <sincostable.h>

namespace gr {
  namespace channels {

    class CHANNELS_API cfo_model_impl : public cfo_model
    {
    private:
      double d_samp_rate;
      double d_std_dev_hz;
      double d_max_dev_hz;
      sincostable d_table;
      gr::analog::fastnoise_source_f::sptr d_noise;
      double d_cfo;
      float d_angle;
      double d_noise_seed;

    public:
      cfo_model_impl(
                double sample_rate_hz,
                double std_dev_hz,
                double max_dev_hz,
                double noise_seed=0 );

      ~cfo_model_impl();
      void setup_rpc();
      int work(int, gr_vector_const_void_star&, gr_vector_void_star&);

      void set_std_dev(double _dev){ d_std_dev_hz = _dev; d_noise = gr::analog::fastnoise_source_f::make(analog::GR_GAUSSIAN, d_std_dev_hz, d_noise_seed); }
      void set_max_dev(double _dev){ d_max_dev_hz = _dev; }
      void set_samp_rate(double _rate){ d_samp_rate = _rate; }

      double std_dev() const { return d_std_dev_hz; }
      double max_dev() const { return d_max_dev_hz; }
      double samp_rate() const { return d_samp_rate; }

    };

  } /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CFO_MODEL_IMPL_H */
