/* -*- c++ -*- */
/*
 * Copyright 2004,2010-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pll_refout_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/sincos.h>
#include <cmath>

namespace gr {
namespace analog {

pll_refout_cc::sptr pll_refout_cc::make(float loop_bw, float max_freq, float min_freq)
{
    return gnuradio::make_block_sptr<pll_refout_cc_impl>(loop_bw, max_freq, min_freq);
}

pll_refout_cc_impl::pll_refout_cc_impl(float loop_bw, float max_freq, float min_freq)
    : sync_block("pll_refout_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      blocks::control_loop(loop_bw, max_freq, min_freq)
{
}

pll_refout_cc_impl::~pll_refout_cc_impl() {}

int pll_refout_cc_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    const gr_complex* iptr = (const gr_complex*)input_items[0];
    gr_complex* optr = (gr_complex*)output_items[0];

    float error;
    float t_imag, t_real;
    int size = noutput_items;

    while (size-- > 0) {
        gr::sincosf(d_phase, &t_imag, &t_real);
        *optr++ = gr_complex(t_real, t_imag);

        error = phase_detector(*iptr++, d_phase);

        advance_loop(error);
        phase_wrap();
        frequency_limit();
    }
    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */
