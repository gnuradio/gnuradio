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

#include "channel_model_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
  namespace channels {

    channel_model::sptr
    channel_model::make(double noise_voltage,
			double frequency_offset,
			double epsilon,
			const std::vector<gr_complex> &taps,
			double noise_seed,
			bool block_tags)
    {
      return gnuradio::get_initial_sptr
	(new channel_model_impl(noise_voltage,
				frequency_offset,
				epsilon,
				taps,
				noise_seed,
				block_tags));
    }

    // Hierarchical block constructor
    channel_model_impl::channel_model_impl(double noise_voltage,
					   double frequency_offset,
					   double epsilon,
					   const std::vector<gr_complex> &taps,
					   double noise_seed,
					   bool block_tags
					   )
      : hier_block2("channel_model",
		       io_signature::make(1, 1, sizeof(gr_complex)),
		       io_signature::make(1, 1, sizeof(gr_complex)))
    {
      d_taps = taps;
      while(d_taps.size() < 2) {
	d_taps.push_back(0);
      }

      d_timing_offset = filter::fractional_resampler_cc::make(0, epsilon);

      d_multipath = filter::fir_filter_ccc::make(1, d_taps);

      d_noise_adder = blocks::add_cc::make();
      d_noise = analog::fastnoise_source_c::make(analog::GR_GAUSSIAN,
						 noise_voltage, noise_seed);
      d_freq_offset = analog::sig_source_c::make(1, analog::GR_SIN_WAVE,
						 frequency_offset, 1.0, 0.0);
      d_mixer_offset = blocks::multiply_cc::make();

      connect(self(), 0, d_timing_offset, 0);
      connect(d_timing_offset, 0, d_multipath, 0);
      connect(d_multipath, 0, d_mixer_offset, 0);
      connect(d_freq_offset, 0, d_mixer_offset, 1);
      connect(d_mixer_offset, 0, d_noise_adder, 1);
      connect(d_noise, 0, d_noise_adder, 0);
      connect(d_noise_adder, 0, self(), 0);

      if (block_tags) {
	d_timing_offset->set_tag_propagation_policy(gr::block::TPP_DONT);
      }
    }

    channel_model_impl::~channel_model_impl()
    {
    }

    void
    channel_model_impl::set_noise_voltage(double noise_voltage)
    {
      d_noise->set_amplitude(noise_voltage);
    }

    void
    channel_model_impl::set_frequency_offset(double frequency_offset)
    {
      d_freq_offset->set_frequency(frequency_offset);
    }

    void
    channel_model_impl::set_taps(const std::vector<gr_complex> &taps)
    {
      d_taps = taps;
      while(d_taps.size() < 2) {
	d_taps.push_back(0);
      }
      d_multipath->set_taps(d_taps);
    }

    void
    channel_model_impl::set_timing_offset(double epsilon)
    {
      d_timing_offset->set_resamp_ratio(epsilon);
    }

    double
    channel_model_impl::noise_voltage() const
    {
      return d_noise->amplitude();
    }

    double
    channel_model_impl::frequency_offset() const
    {
      return d_freq_offset->frequency();
    }

    std::vector<gr_complex>
    channel_model_impl::taps() const
    {
      return d_multipath->taps();
    }

    double
    channel_model_impl::timing_offset() const
    {
      return d_timing_offset->resamp_ratio();
    }

    void
    channel_model_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<channel_model, double>(
	  alias(), "noise",
	  &channel_model::noise_voltage,
	  pmt::mp(-10.0f), pmt::mp(10.0f), pmt::mp(0.0f),
	  "", "Noise Voltage", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTSTRIP)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<channel_model, double>(
           alias(), "freq",
	   &channel_model::frequency_offset,
	   pmt::mp(-1.0f), pmt::mp(1.0f), pmt::mp(0.0f),
	   "Hz", "Frequency Offset", RPC_PRIVLVL_MIN,
           DISPTIME | DISPOPTSTRIP)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<channel_model, double>(
	  alias(), "timing",
	  &channel_model::timing_offset,
	  pmt::mp(0.0), pmt::mp(2.0), pmt::mp(0.0),
	  "", "Timing Offset", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTSTRIP)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<channel_model, std::vector<gr_complex> >(
	  alias(), "taps",
	  &channel_model::taps,
	  pmt::make_c32vector(0,-10),
	  pmt::make_c32vector(0,10),
	  pmt::make_c32vector(0,0),
	  "", "Multipath taps", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTCPLX | DISPOPTSTRIP)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<channel_model, double>(
	  alias(), "noise",
	  &channel_model::set_noise_voltage,
	  pmt::mp(-10.0), pmt::mp(10.0), pmt::mp(0.0),
	  "V", "Noise Voltage",
	  RPC_PRIVLVL_MIN, DISPNULL)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<channel_model, double>(
	  alias(), "freq",
	  &channel_model::set_frequency_offset,
	  pmt::mp(-1.0), pmt::mp(1.0), pmt::mp(0.0),
	  "Hz", "Frequency Offset",
	  RPC_PRIVLVL_MIN, DISPNULL)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<channel_model, double>(
	  alias(), "timing",
	  &channel_model::set_timing_offset,
	  pmt::mp(0.0f), pmt::mp(2.0f), pmt::mp(0.0f),
	  "", "Timing Offset",
	  RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace channels */
} /* namespace gr */
