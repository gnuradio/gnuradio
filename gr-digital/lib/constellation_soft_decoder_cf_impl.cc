/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "constellation_soft_decoder_cf_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace digital {

constellation_soft_decoder_cf::sptr
constellation_soft_decoder_cf::make(constellation_sptr constellation)
{
    return gnuradio::get_initial_sptr(
        new constellation_soft_decoder_cf_impl(constellation));
}

constellation_soft_decoder_cf_impl::constellation_soft_decoder_cf_impl(
    constellation_sptr constellation)
    : sync_interpolator("constellation_soft_decoder_cf",
                        io_signature::make(1, 1, sizeof(gr_complex)),
                        io_signature::make(1, 1, sizeof(float)),
                        constellation->bits_per_symbol()),
      d_constellation(constellation),
      d_dim(constellation->dimensionality()),
      d_bps(constellation->bits_per_symbol())
{
}

constellation_soft_decoder_cf_impl::~constellation_soft_decoder_cf_impl() {}

int constellation_soft_decoder_cf_impl::work(int noutput_items,
                                             gr_vector_const_void_star& input_items,
                                             gr_vector_void_star& output_items)
{
    gr_complex const* in = (const gr_complex*)input_items[0];
    float* out = (float*)output_items[0];

    std::vector<float> bits;

    // FIXME: figure out how to manage d_dim
    for (int i = 0; i < noutput_items / d_bps; i++) {
        bits = d_constellation->soft_decision_maker(in[i]);
        for (size_t j = 0; j < bits.size(); j++) {
            out[d_bps * i + j] = bits[j];
        }
    }

    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
