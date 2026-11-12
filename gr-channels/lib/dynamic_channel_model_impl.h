/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_DYNAMIC_CHANNEL_MODEL_IMPL_H
#define INCLUDED_CHANNELS_DYNAMIC_CHANNEL_MODEL_IMPL_H

#include <gnuradio/analog/fastnoise_source.h>
#include <gnuradio/analog/sig_source.h>
#include <gnuradio/blocks/add_blk.h>
#include <gnuradio/blocks/multiply.h>
#include <gnuradio/channels/cfo_model.h>
#include <gnuradio/channels/dynamic_channel_model.h>
#include <gnuradio/channels/selective_fading_model.h>
#include <gnuradio/channels/sro_model.h>
#include <gnuradio/filter/fir_filter_blk.h>
#include <gnuradio/filter/mmse_resampler_cc.h>
#include <gnuradio/top_block.h>

namespace gr {
namespace channels {

class CHANNELS_API dynamic_channel_model_impl : public dynamic_channel_model
{
private:
    channels::sro_model::sptr d_sro_model;
    channels::cfo_model::sptr d_cfo_model;
    channels::selective_fading_model::sptr d_fader;
    blocks::add_cc::sptr d_noise_adder;
    analog::fastnoise_source_c::sptr d_noise;

public:
    dynamic_channel_model_impl(double samp_rate,
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
                               double noise_seed);

    ~dynamic_channel_model_impl() override;

    void setup_rpc() override;

    double samp_rate() const override;
    double sro_dev_std() const override;
    double sro_dev_max() const override;
    double cfo_dev_std() const override;
    double cfo_dev_max() const override;
    double noise_amp() const override;
    double doppler_freq() const override;
    double K() const override;

    void set_samp_rate(double sr) override;
    void set_sro_dev_std(double dev) override;
    void set_sro_dev_max(double dev) override;
    void set_cfo_dev_std(double dev) override;
    void set_cfo_dev_max(double dev) override;
    void set_noise_amp(double amp) override;
    void set_doppler_freq(double f) override;
    void set_K(double K) override;
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_DYNAMIC_CHANNEL_MODEL_IMPL_H */
