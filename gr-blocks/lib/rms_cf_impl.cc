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

#include "rms_cf_impl.h"
#include <gnuradio/io_signature.h>
#include <cmath>

namespace gr {
namespace blocks {

rms_cf::sptr rms_cf::make(double alpha)
{
    return gnuradio::make_block_sptr<rms_cf_impl>(alpha);
}

rms_cf_impl::rms_cf_impl(double alpha)
    : sync_block("rms_cf",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(float)))
{
    set_alpha(alpha);
}

rms_cf_impl::~rms_cf_impl() {}

void rms_cf_impl::set_alpha(double alpha)
{
    d_alpha = alpha;
    d_beta = 1 - d_alpha;
    d_avg = 0;
}

int rms_cf_impl::work(int noutput_items,
                      gr_vector_const_void_star& input_items,
                      gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    float* out = (float*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        double mag_sqrd = in[i].real() * in[i].real() + in[i].imag() * in[i].imag();
        d_avg = d_beta * d_avg + d_alpha * mag_sqrd;
        out[i] = sqrt(d_avg);
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
