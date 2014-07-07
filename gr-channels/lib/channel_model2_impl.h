/* -*- c++ -*- */
/*
 * Copyright 2009,2012,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CHANNELS_CHANNEL_MODEL2_IMPL_H
#define INCLUDED_CHANNELS_CHANNEL_MODEL2_IMPL_H

#include <gnuradio/channels/channel_model2.h>
#include <gnuradio/top_block.h>
#include <gnuradio/blocks/add_cc.h>
#include <gnuradio/blocks/multiply_cc.h>
#include <gnuradio/analog/sig_source_c.h>
#include <gnuradio/analog/fastnoise_source_c.h>
#include <gnuradio/filter/fractional_resampler_cc.h>
#include <gnuradio/filter/fir_filter_ccc.h>
#include <gnuradio/blocks/vco_c.h>

namespace gr {
  namespace channels {

    class CHANNELS_API channel_model2_impl : public channel_model2
    {
    private:
      blocks::add_cc::sptr d_noise_adder;
      blocks::multiply_cc::sptr d_mixer_offset;

      blocks::vco_c::sptr d_freq_gen;

      analog::fastnoise_source_c::sptr d_noise;

      filter::fractional_resampler_cc::sptr d_timing_offset;
      filter::fir_filter_ccc::sptr d_multipath;

      std::vector<gr_complex> d_taps;

    public:
      channel_model2_impl(double noise_voltage,
                          double epsilon,
                          const std::vector<gr_complex> &taps,
                          double noise_seed,
			  bool block_tags);

      ~channel_model2_impl();

      void setup_rpc();

      void set_noise_voltage(double noise_voltage);
      void set_taps(const std::vector<gr_complex> &taps);
      void set_timing_offset(double epsilon);

      double noise_voltage() const;
      std::vector<gr_complex> taps() const;
      double timing_offset() const;
    };

  } /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CHANNEL_MODEL2_IMPL_H */
