/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H
#define INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H

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
    ~fading_model_impl();
    void setup_rpc();
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    virtual float fDTs() { return d_fader.d_fDTs; }
    virtual float K() { return d_fader.d_K; }
    virtual float step() { return d_fader.d_step; }

    virtual void set_fDTs(float fDTs)
    {
        d_fader.d_fDTs = fDTs;
        d_fader.d_step = powf(0.00125 * fDTs, 1.1);
    }
    virtual void set_K(float K)
    {
        d_fader.d_K = K;
        d_fader.scale_los = sqrtf(d_fader.d_K) / sqrtf(d_fader.d_K + 1);
        d_fader.scale_nlos = (1 / sqrtf(d_fader.d_K + 1));
    }
    virtual void set_step(float step) { d_fader.d_step = step; }
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CHANNEL_MODEL_IMPL_H */
