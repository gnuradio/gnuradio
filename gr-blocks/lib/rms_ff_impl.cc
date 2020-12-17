/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rms_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <cmath>

namespace gr {
namespace blocks {

rms_ff::sptr rms_ff::make(double alpha)
{
    return gnuradio::make_block_sptr<rms_ff_impl>(alpha);
}

rms_ff_impl::rms_ff_impl(double alpha)
    : sync_block("rms_ff",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float)))
{
    set_alpha(alpha);
}

rms_ff_impl::~rms_ff_impl() {}

void rms_ff_impl::set_alpha(double alpha)
{
    d_alpha = alpha;
    d_beta = 1 - d_alpha;
    d_avg = 0;
}

int rms_ff_impl::work(int noutput_items,
                      gr_vector_const_void_star& input_items,
                      gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        double mag_sqrd = in[i] * in[i];
        d_avg = d_beta * d_avg + d_alpha * mag_sqrd;
        out[i] = sqrt(d_avg);
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
