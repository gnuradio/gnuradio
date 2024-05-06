/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quadrature_demod_cf_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <volk/volk.h>

namespace gr {
namespace analog {

quadrature_demod_cf::sptr quadrature_demod_cf::make(float gain)
{
    return gnuradio::make_block_sptr<quadrature_demod_cf_impl>(gain);
}

quadrature_demod_cf_impl::quadrature_demod_cf_impl(float gain)
    : sync_block("quadrature_demod_cf",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(float))),
      d_gain(gain)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));

    set_history(2); // we need to look at the previous value
}

quadrature_demod_cf_impl::~quadrature_demod_cf_impl() {}

int quadrature_demod_cf_impl::work(int noutput_items,
                                   gr_vector_const_void_star& input_items,
                                   gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    float* out = (float*)output_items[0];

    std::vector<gr_complex> tmp(noutput_items);
    volk_32fc_x2_multiply_conjugate_32fc(&tmp[0], &in[1], &in[0], noutput_items);
    for (int i = 0; i < noutput_items; i++) {
        out[i] = d_gain * gr::fast_atan2f(imag(tmp[i]), real(tmp[i]));
    }

    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */
