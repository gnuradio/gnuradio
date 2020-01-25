/* -*- c++ -*- */
/*
 * Copyright 2014,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "atsc_fpll_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <gnuradio/sincos.h>

namespace gr {
namespace dtv {

atsc_fpll::sptr atsc_fpll::make(float rate)
{
    return gnuradio::get_initial_sptr(new atsc_fpll_impl(rate));
}

atsc_fpll_impl::atsc_fpll_impl(float rate)
    : sync_block("dtv_atsc_fpll",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(float)))
{
    d_afc.set_taps(1.0 - exp(-1.0 / rate / 5e-6));
    d_nco.set_freq((-3e6 + 0.309e6) / rate * 2 * GR_M_PI);
    d_nco.set_phase(0.0);
}

atsc_fpll_impl::~atsc_fpll_impl() {}

int atsc_fpll_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    float* out = (float*)output_items[0];

    for (int k = 0; k < noutput_items; k++) {
        float a_cos, a_sin;

        d_nco.step();                 // increment phase
        d_nco.sincos(&a_sin, &a_cos); // compute cos and sin

        // Mix out carrier and output I-only signal
        gr_complex result = in[k] * gr_complex(a_sin, a_cos);
        out[k] = result.real();

        // Update phase/freq error
        gr_complex filtered = d_afc.filter(result);
        float x = gr::fast_atan2f(filtered.imag(), filtered.real());

        // avoid slamming filter with big transitions
        static const float limit = GR_M_PI / 2.0;
        if (x > limit)
            x = limit;
        else if (x < -limit)
            x = -limit;

        static const float alpha = 0.01;
        static const float beta = alpha * alpha / 4.0;
        d_nco.adjust_phase(alpha * x);
        d_nco.adjust_freq(beta * x);
    }

    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */
