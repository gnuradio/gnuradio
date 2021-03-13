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
    const gr_complex* iptr = (gr_complex*)input_items[0];
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

void pll_refout_cc_impl::set_loop_bandwidth(float bw)
{
    blocks::control_loop::set_loop_bandwidth(bw);
}

void pll_refout_cc_impl::set_damping_factor(float df)
{
    blocks::control_loop::set_damping_factor(df);
}

void pll_refout_cc_impl::set_alpha(float alpha)
{
    blocks::control_loop::set_alpha(alpha);
}

void pll_refout_cc_impl::set_beta(float beta) { blocks::control_loop::set_beta(beta); }

void pll_refout_cc_impl::set_frequency(float freq)
{
    blocks::control_loop::set_frequency(freq);
}

void pll_refout_cc_impl::set_phase(float phase)
{
    blocks::control_loop::set_phase(phase);
}

void pll_refout_cc_impl::set_min_freq(float freq)
{
    blocks::control_loop::set_min_freq(freq);
}

void pll_refout_cc_impl::set_max_freq(float freq)
{
    blocks::control_loop::set_max_freq(freq);
}


float pll_refout_cc_impl::get_loop_bandwidth() const
{
    return blocks::control_loop::get_loop_bandwidth();
}

float pll_refout_cc_impl::get_damping_factor() const
{
    return blocks::control_loop::get_damping_factor();
}

float pll_refout_cc_impl::get_alpha() const { return blocks::control_loop::get_alpha(); }

float pll_refout_cc_impl::get_beta() const { return blocks::control_loop::get_beta(); }

float pll_refout_cc_impl::get_frequency() const
{
    return blocks::control_loop::get_frequency();
}

float pll_refout_cc_impl::get_phase() const { return blocks::control_loop::get_phase(); }

float pll_refout_cc_impl::get_min_freq() const
{
    return blocks::control_loop::get_min_freq();
}

float pll_refout_cc_impl::get_max_freq() const
{
    return blocks::control_loop::get_max_freq();
}

} /* namespace analog */
} /* namespace gr */
