/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "feedforward_agc_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace analog {

feedforward_agc_cc::sptr feedforward_agc_cc::make(int nsamples, float reference)
{
    return gnuradio::make_block_sptr<feedforward_agc_cc_impl>(nsamples, reference);
}

feedforward_agc_cc_impl::feedforward_agc_cc_impl(int nsamples, float reference)
    : sync_block("feedforward_agc_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_nsamples(nsamples),
      d_reference(reference)
{
    if (nsamples < 1)
        throw std::invalid_argument("feedforward_agc_cc_impl: nsamples must be >= 1");

    set_history(nsamples);
}

feedforward_agc_cc_impl::~feedforward_agc_cc_impl() {}

// approximate sqrt(x^2 + y^2)
inline static float envelope(gr_complex x)
{
    float r_abs = std::fabs(x.real());
    float i_abs = std::fabs(x.imag());

    if (r_abs > i_abs)
        return r_abs + 0.4 * i_abs;
    else
        return i_abs + 0.4 * r_abs;
}

int feedforward_agc_cc_impl::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];
    int nsamples = d_nsamples;
    float gain;

    for (int i = 0; i < noutput_items; i++) {
        // float max_env = 1e-12;	// avoid divide by zero
        float max_env = 1e-4; // avoid divide by zero, indirectly set max gain
        for (int j = 0; j < nsamples; j++) {
            max_env = std::max(max_env, envelope(in[i + j]));
        }
        gain = d_reference / max_env;
        out[i] = gain * in[i];
    }
    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */
