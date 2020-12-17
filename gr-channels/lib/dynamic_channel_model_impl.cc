/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "dynamic_channel_model_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
namespace channels {

dynamic_channel_model::sptr dynamic_channel_model::make(double samp_rate,
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
                                                        double noise_seed)
{
    return gnuradio::make_block_sptr<dynamic_channel_model_impl>(samp_rate,
                                                                 sro_std_dev,
                                                                 sro_max_dev,
                                                                 cfo_std_dev,
                                                                 cfo_max_dev,
                                                                 N,
                                                                 doppler_freq,
                                                                 LOS_model,
                                                                 K,
                                                                 delays,
                                                                 mags,
                                                                 ntaps_mpath,
                                                                 noise_amp,
                                                                 noise_seed);
}

// Hierarchical block constructor
dynamic_channel_model_impl::dynamic_channel_model_impl(double samp_rate,
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
                                                       double noise_seed)
    : hier_block2("dynamic_channel_model",
                  io_signature::make(1, 1, sizeof(gr_complex)),
                  io_signature::make(1, 1, sizeof(gr_complex)))
{
    d_noise_adder = blocks::add_cc::make();
    d_noise = analog::fastnoise_source_c::make(
        analog::GR_GAUSSIAN, noise_amp, noise_seed, 1024 * 8);
    d_sro_model =
        channels::sro_model::make(samp_rate, sro_std_dev, sro_max_dev, noise_seed);
    d_cfo_model =
        channels::cfo_model::make(samp_rate, cfo_std_dev, cfo_max_dev, noise_seed);
    d_fader = channels::selective_fading_model::make(
        N, doppler_freq / samp_rate, LOS_model, K, noise_seed, delays, mags, ntaps_mpath);

    connect(self(), 0, d_sro_model, 0);
    connect(d_sro_model, 0, d_cfo_model, 0);
    connect(d_cfo_model, 0, d_fader, 0);
    connect(d_fader, 0, d_noise_adder, 1);
    connect(d_noise, 0, d_noise_adder, 0);
    connect(d_noise_adder, 0, self(), 0);
}

dynamic_channel_model_impl::~dynamic_channel_model_impl() {}

void dynamic_channel_model_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
#endif /* GR_CTRLPORT */
}

} /* namespace channels */
} /* namespace gr */
