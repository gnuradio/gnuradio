/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2011,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pll_freqdet_cf_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

#include <cmath>

namespace gr {
namespace analog {

pll_freqdet_cf::sptr pll_freqdet_cf::make(float loop_bw, float max_freq, float min_freq)
{
    return gnuradio::make_block_sptr<pll_freqdet_cf_impl>(loop_bw, max_freq, min_freq);
}

pll_freqdet_cf_impl::pll_freqdet_cf_impl(float loop_bw, float max_freq, float min_freq)
    : sync_block("pll_freqdet_cf",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(float))),
      blocks::control_loop(loop_bw, max_freq, min_freq)
{
}

pll_freqdet_cf_impl::~pll_freqdet_cf_impl() {}
constexpr float mod_2pi(float in)
{
    if (in > GR_M_PI) {
        return in - static_cast<float>(2.0 * GR_M_PI);
    }
    if (in < -GR_M_PI) {
        return in + static_cast<float>(2.0 * GR_M_PI);
    }
    return in;
}

float phase_detector(gr_complex sample, float ref_phase)
{
    float sample_phase = gr::fast_atan2f(sample.imag(), sample.real());
    return mod_2pi(sample_phase - ref_phase);
}

int pll_freqdet_cf_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const gr_complex* iptr = (const gr_complex*)input_items[0];
    float* optr = (float*)output_items[0];

    float error;
    int size = noutput_items;

    while (size-- > 0) {
        *optr++ = d_freq;

        error = phase_detector(*iptr++, d_phase);

        advance_loop(error);
        phase_wrap();
        frequency_limit();
    }
    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */
