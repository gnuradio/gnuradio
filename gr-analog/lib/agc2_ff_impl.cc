/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "agc2_ff_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace analog {

agc2_ff::sptr agc2_ff::make(
    float attack_rate, float decay_rate, float reference, float gain, float max_gain)
{
    return gnuradio::make_block_sptr<agc2_ff_impl>(
        attack_rate, decay_rate, reference, gain, max_gain);
}

agc2_ff_impl::~agc2_ff_impl() {}

agc2_ff_impl::agc2_ff_impl(
    float attack_rate, float decay_rate, float reference, float gain, float max_gain)
    : sync_block("agc2_ff",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float))),
      kernel::agc2_ff(attack_rate, decay_rate, reference, gain, max_gain)
{
}

int agc2_ff_impl::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];
    scaleN(out, in, noutput_items);
    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */
