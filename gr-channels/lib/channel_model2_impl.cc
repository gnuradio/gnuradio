/* -*- c++ -*- */
/*
 * Copyright 2009,2012,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "channel_model2_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <iostream>

namespace gr {
namespace channels {

channel_model2::sptr channel_model2::make(double noise_voltage,
                                          double epsilon,
                                          const std::vector<gr_complex>& taps,
                                          double noise_seed,
                                          bool block_tags)
{
    return gnuradio::make_block_sptr<channel_model2_impl>(
        noise_voltage, epsilon, taps, noise_seed, block_tags);
}

// Hierarchical block constructor
channel_model2_impl::channel_model2_impl(double noise_voltage,
                                         double epsilon,
                                         const std::vector<gr_complex>& taps,
                                         double noise_seed,
                                         bool block_tags)
    : hier_block2("channel_model2",
                  io_signature::make2(3, 3, sizeof(gr_complex), sizeof(float)),
                  io_signature::make(1, 1, sizeof(gr_complex)))
{
    d_taps = taps;
    while (d_taps.size() < 2) {
        d_taps.push_back(0);
    }

    d_timing_offset = filter::mmse_resampler_cc::make(0, epsilon);

    d_multipath = filter::fir_filter_ccc::make(1, d_taps);

    d_noise_adder = blocks::add_cc::make();
    d_noise =
        analog::fastnoise_source_c::make(analog::GR_GAUSSIAN, noise_voltage, noise_seed);
    d_freq_gen = blocks::vco_c::make(1.0, 2 * GR_M_PI, 1.0);

    d_mixer_offset = blocks::multiply_cc::make();

    connect(self(), 0, d_timing_offset, 0);
    connect(self(), 2, d_timing_offset, 1);
    connect(d_timing_offset, 0, d_multipath, 0);
    connect(d_multipath, 0, d_mixer_offset, 0);

    connect(self(), 1, d_freq_gen, 0);
    connect(d_freq_gen, 0, d_mixer_offset, 1);

    connect(d_mixer_offset, 0, d_noise_adder, 1);
    connect(d_noise, 0, d_noise_adder, 0);
    connect(d_noise_adder, 0, self(), 0);

    if (block_tags) {
        d_timing_offset->set_tag_propagation_policy(gr::block::TPP_DONT);
    }
}

channel_model2_impl::~channel_model2_impl() {}

void channel_model2_impl::set_noise_voltage(double noise_voltage)
{
    d_noise->set_amplitude(noise_voltage);
}

void channel_model2_impl::set_taps(const std::vector<gr_complex>& taps)
{
    d_taps = taps;
    while (d_taps.size() < 2) {
        d_taps.push_back(0);
    }
    d_multipath->set_taps(d_taps);
}

void channel_model2_impl::set_timing_offset(double epsilon)
{
    d_timing_offset->set_resamp_ratio(epsilon);
}

double channel_model2_impl::noise_voltage() const { return d_noise->amplitude(); }

std::vector<gr_complex> channel_model2_impl::taps() const { return d_multipath->taps(); }

double channel_model2_impl::timing_offset() const
{
    return d_timing_offset->resamp_ratio();
}

void channel_model2_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<channel_model2, double>(alias(),
                                                          "noise",
                                                          &channel_model2::noise_voltage,
                                                          pmt::mp(-10.0f),
                                                          pmt::mp(10.0f),
                                                          pmt::mp(0.0f),
                                                          "",
                                                          "Noise Voltage",
                                                          RPC_PRIVLVL_MIN,
                                                          DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<channel_model2, double>(alias(),
                                                          "timing",
                                                          &channel_model2::timing_offset,
                                                          pmt::mp(0.0),
                                                          pmt::mp(2.0),
                                                          pmt::mp(0.0),
                                                          "",
                                                          "Timing Offset",
                                                          RPC_PRIVLVL_MIN,
                                                          DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<channel_model2, std::vector<gr_complex>>(
            alias(),
            "taps",
            &channel_model2::taps,
            pmt::make_c32vector(0, -10),
            pmt::make_c32vector(0, 10),
            pmt::make_c32vector(0, 0),
            "",
            "Multipath taps",
            RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTCPLX | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_set<channel_model2, double>(
        alias(),
        "noise",
        &channel_model2::set_noise_voltage,
        pmt::mp(-10.0),
        pmt::mp(10.0),
        pmt::mp(0.0),
        "V",
        "Noise Voltage",
        RPC_PRIVLVL_MIN,
        DISPNULL)));

    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_set<channel_model2, double>(
        alias(),
        "timing",
        &channel_model2::set_timing_offset,
        pmt::mp(0.0f),
        pmt::mp(2.0f),
        pmt::mp(0.0f),
        "",
        "Timing Offset",
        RPC_PRIVLVL_MIN,
        DISPNULL)));
#endif /* GR_CTRLPORT */
}

} /* namespace channels */
} /* namespace gr */
