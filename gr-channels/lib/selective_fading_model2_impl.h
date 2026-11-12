/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL2_IMPL_H
#define INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL2_IMPL_H

#include "flat_fader_impl.h"
#include <gnuradio/channels/selective_fading_model2.h>
#include <gnuradio/random.h>
#include <gnuradio/sync_block.h>

#include <gnuradio/fxpt.h>

namespace gr {
namespace channels {

class CHANNELS_API selective_fading_model2_impl : public selective_fading_model2
{
private:
    std::vector<gr::channels::flat_fader_impl> d_faders;
    std::vector<std::vector<gr_complex>> d_fading_taps; // per-path gains for one call
    std::vector<float> d_delays;
    std::vector<float> d_delays_orig;
    std::vector<float> d_delays_std;
    std::vector<float> d_delays_maxdev;
    std::vector<float> d_mags;

    gr::random d_delay_rng; // delay random walk increments, N(0, 1)

public:
    selective_fading_model2_impl(unsigned int N,
                                 float fDTs,
                                 bool LOS,
                                 float K,
                                 uint32_t seed,
                                 std::vector<float> delays,
                                 std::vector<float> delays_std,
                                 std::vector<float> delays_maxdev,
                                 std::vector<float> mags,
                                 unsigned int ntaps);
    ~selective_fading_model2_impl() override;

    // Disallow copy. This is a heavy object.
    selective_fading_model2_impl(const selective_fading_model2_impl&) = delete;
    selective_fading_model2_impl& operator=(const selective_fading_model2_impl&) = delete;

    void setup_rpc() override;
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
    std::vector<gr_complex> d_taps;

    float fDTs() override;
    float K() override;
    float step() override;

    void set_fDTs(float fDTs) override;
    void set_K(float K) override;
    void set_step(float step) override;
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL2_IMPL_H */
