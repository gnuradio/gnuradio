/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H
#define INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H

#include <gnuradio/analog/fastnoise_source.h>
#include <gnuradio/analog/sig_source.h>
#include <gnuradio/blocks/add_blk.h>
#include <gnuradio/blocks/multiply.h>
#include <gnuradio/channels/channel_model.h>
#include <gnuradio/filter/fir_filter_blk.h>
#include <gnuradio/filter/mmse_resampler_cc.h>
#include <gnuradio/top_block.h>

namespace gr {
namespace channels {

class CHANNELS_API channel_model_impl : public channel_model
{
private:
    blocks::add_cc::sptr d_noise_adder;
    blocks::multiply_cc::sptr d_mixer_offset;

    analog::sig_source_c::sptr d_freq_offset;
    analog::fastnoise_source_c::sptr d_noise;

    filter::mmse_resampler_cc::sptr d_timing_offset;
    filter::fir_filter_ccc::sptr d_multipath;

    std::vector<gr_complex> d_taps;

public:
    channel_model_impl(double noise_voltage,
                       double frequency_offset,
                       double epsilon,
                       const std::vector<gr_complex>& taps,
                       double noise_seed,
                       bool block_tags);

    ~channel_model_impl() override;

    void setup_rpc() override;

    void set_noise_voltage(double noise_voltage) override;
    void set_frequency_offset(double frequency_offset) override;
    void set_taps(const std::vector<gr_complex>& taps) override;
    void set_timing_offset(double epsilon) override;

    double noise_voltage() const override;
    double frequency_offset() const override;
    std::vector<gr_complex> taps() const override;
    double timing_offset() const override;
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H */
