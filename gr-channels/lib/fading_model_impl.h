/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_FADING_MODEL_IMPL_H
#define INCLUDED_CHANNELS_FADING_MODEL_IMPL_H

#include "flat_fader_impl.h"
#include <gnuradio/channels/fading_model.h>
#include <gnuradio/sync_block.h>

#include "sincostable.h"
#include <gnuradio/fxpt.h>

namespace gr {
namespace channels {

class CHANNELS_API fading_model_impl : public fading_model
{
private:
    gr::channels::flat_fader_impl d_fader;

public:
    fading_model_impl(unsigned int N, float fDTs, bool LOS, float K, uint32_t seed);
    ~fading_model_impl() override;
    void setup_rpc() override;
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    float fDTs() override { return d_fader.d_fDTs; }
    float K() override { return d_fader.d_K; }
    float step() override { return d_fader.d_step; }

    void set_fDTs(float fDTs) override
    {
        d_fader.d_fDTs = fDTs;
        d_fader.d_step = powf(0.00125 * fDTs, 1.1);
    }
    void set_K(float K) override
    {
        d_fader.d_K = K;
        d_fader.scale_los = sqrtf(d_fader.d_K) / sqrtf(d_fader.d_K + 1);
        d_fader.scale_nlos = (1 / sqrtf(d_fader.d_K + 1));
    }
    void set_step(float step) override { d_fader.d_step = step; }
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_FADING_MODEL_IMPL_H */
