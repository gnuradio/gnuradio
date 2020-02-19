/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_CFO_MODEL_IMPL_H
#define INCLUDED_CHANNELS_CFO_MODEL_IMPL_H

#include "sincostable.h"
#include <gnuradio/analog/fastnoise_source.h>
#include <gnuradio/blocks/integrate.h>
#include <gnuradio/blocks/multiply.h>
#include <gnuradio/blocks/vco_c.h>
#include <gnuradio/channels/cfo_model.h>
#include <gnuradio/top_block.h>

namespace gr {
namespace channels {

class CHANNELS_API cfo_model_impl : public cfo_model
{
private:
    double d_samp_rate;
    double d_std_dev_hz;
    double d_max_dev_hz;
    sincostable d_table;
    gr::analog::fastnoise_source_f::sptr d_noise;
    double d_cfo;
    float d_angle;
    double d_noise_seed;

public:
    cfo_model_impl(double sample_rate_hz,
                   double std_dev_hz,
                   double max_dev_hz,
                   double noise_seed = 0);

    ~cfo_model_impl();
    void setup_rpc();
    int work(int, gr_vector_const_void_star&, gr_vector_void_star&);

    void set_std_dev(double _dev)
    {
        d_std_dev_hz = _dev;
        d_noise = gr::analog::fastnoise_source_f::make(
            analog::GR_GAUSSIAN, d_std_dev_hz, d_noise_seed);
    }
    void set_max_dev(double _dev) { d_max_dev_hz = _dev; }
    void set_samp_rate(double _rate) { d_samp_rate = _rate; }

    double std_dev() const { return d_std_dev_hz; }
    double max_dev() const { return d_max_dev_hz; }
    double samp_rate() const { return d_samp_rate; }
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CFO_MODEL_IMPL_H */
