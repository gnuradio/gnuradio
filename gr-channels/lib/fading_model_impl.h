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
    std::vector<gr_complex> d_ftaps;

public:
    fading_model_impl(unsigned int N, float fDTs, bool LOS, float K, uint32_t seed);
    ~fading_model_impl() override;
    void setup_rpc() override;
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    float fDTs() override;
    float K() override;
    float step() override;

    void set_fDTs(float fDTs) override;
    void set_K(float K) override;
    void set_step(float step) override;
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_FADING_MODEL_IMPL_H */
