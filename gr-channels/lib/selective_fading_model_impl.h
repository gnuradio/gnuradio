/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL_IMPL_H
#define INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL_IMPL_H

#include "flat_fader_impl.h"
#include <gnuradio/channels/selective_fading_model.h>
#include <gnuradio/sync_block.h>

#include "sincostable.h"
#include <gnuradio/fxpt.h>

namespace gr {
namespace channels {

class CHANNELS_API selective_fading_model_impl : public selective_fading_model
{
private:
    std::vector<gr::channels::flat_fader_impl> d_faders;
    std::vector<float> d_delays;
    std::vector<float> d_mags;
    sincostable d_sintable;

public:
    selective_fading_model_impl(unsigned int N,
                                float fDTs,
                                bool LOS,
                                float K,
                                int seed,
                                std::vector<float> delays,
                                std::vector<float> mags,
                                int ntaps);
    ~selective_fading_model_impl() override;

    // Disallow copy. This is a heavy object.
    selective_fading_model_impl(const selective_fading_model_impl&) = delete;
    selective_fading_model_impl& operator=(const selective_fading_model_impl&) = delete;

    void setup_rpc() override;
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
    std::vector<gr_complex> d_taps;

    float fDTs() override { return d_faders[0].d_fDTs; }
    float K() override { return d_faders[0].d_K; }
    float step() override { return d_faders[0].d_step; }

    void set_fDTs(float fDTs) override
    {
        for (auto& fader : d_faders) {
            fader.d_fDTs = fDTs;
            fader.d_step = powf(0.00125 * fDTs, 1.1);
        }
    }
    void set_K(float K) override
    {
        for (auto& fader : d_faders) {
            fader.d_K = K;
            fader.scale_los = sqrtf(fader.d_K) / sqrtf(fader.d_K + 1);
            fader.scale_nlos = (1 / sqrtf(fader.d_K + 1));
        }
    }
    void set_step(float step) override
    {
        for (auto& fader : d_faders) {
            fader.d_step = step;
        }
    }
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL_IMPL_H */
