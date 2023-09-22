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
    return gnuradio::make_block_sptr<atsc_fpll_impl>(rate);
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
    constexpr float alpha = 0.01;
    constexpr float beta = alpha * alpha / 4.0;

    auto in = static_cast<const gr_complex*>(input_items[0]);
    auto out = static_cast<float*>(output_items[0]);

    float a_cos, a_sin;
    float x;
    gr_complex result, filtered;

    for (int k = 0; k < noutput_items; k++) {
        d_nco.step();                 // increment phase
        d_nco.sincos(&a_sin, &a_cos); // compute cos and sin

        // Mix out carrier and output I-only signal
        gr::fast_cc_multiply(result, in[k], gr_complex(a_sin, a_cos));

        out[k] = result.real();

        // Update phase/freq error
        filtered = d_afc.filter(result);
        x = gr::fast_atan2f(filtered.imag(), filtered.real());

        // avoid slamming filter with big transitions
        if (x > M_PI_2)
            x = M_PI_2;
        else if (x < -M_PI_2)
            x = -M_PI_2;

        d_nco.adjust_phase(alpha * x);
        d_nco.adjust_freq(beta * x);
    }

    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */
