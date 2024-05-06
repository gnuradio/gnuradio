/* -*- c++ -*- */
/*
 * Copyright 2006,2010-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pll_carriertracking_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/sincos.h>

#include <cmath>

namespace gr {
namespace analog {

pll_carriertracking_cc::sptr
pll_carriertracking_cc::make(float loop_bw, float max_freq, float min_freq)
{
    return gnuradio::make_block_sptr<pll_carriertracking_cc_impl>(
        loop_bw, max_freq, min_freq);
}

pll_carriertracking_cc_impl::pll_carriertracking_cc_impl(float loop_bw,
                                                         float max_freq,
                                                         float min_freq)
    : sync_block("pll_carriertracking_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      blocks::control_loop(loop_bw, max_freq, min_freq),
      d_locksig(0),
      d_lock_threshold(0),
      d_squelch_enable(false)
{
}

pll_carriertracking_cc_impl::~pll_carriertracking_cc_impl() {}

bool pll_carriertracking_cc_impl::squelch_enable(bool set_squelch)
{
    return d_squelch_enable = set_squelch;
}

float pll_carriertracking_cc_impl::set_lock_threshold(float threshold)
{
    return d_lock_threshold = threshold;
}

int pll_carriertracking_cc_impl::work(int noutput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    const gr_complex* iptr = (const gr_complex*)input_items[0];
    gr_complex* optr = (gr_complex*)output_items[0];

    float error;
    float t_imag, t_real;

    for (int i = 0; i < noutput_items; i++) {
        gr::sincosf(d_phase, &t_imag, &t_real);

        fast_cc_multiply(optr[i], iptr[i], gr_complex(t_real, -t_imag));

        error = phase_detector(iptr[i], d_phase);

        advance_loop(error);
        phase_wrap();
        frequency_limit();

        d_locksig = d_locksig * (1.0 - d_alpha) +
                    d_alpha * (iptr[i].real() * t_real + iptr[i].imag() * t_imag);

        if ((d_squelch_enable) && !lock_detector())
            optr[i] = 0;
    }
    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */
