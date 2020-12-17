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

#include "agc_ff_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace analog {

agc_ff::sptr agc_ff::make(float rate, float reference, float gain)
{
    return gnuradio::make_block_sptr<agc_ff_impl>(rate, reference, gain);
}

agc_ff_impl::agc_ff_impl(float rate, float reference, float gain)
    : sync_block("agc_ff",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float))),
      kernel::agc_ff(rate, reference, gain, 65536)
{
}

agc_ff_impl::~agc_ff_impl() {}

int agc_ff_impl::work(int noutput_items,
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
