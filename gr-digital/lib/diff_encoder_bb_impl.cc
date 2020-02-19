/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "diff_encoder_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace digital {

diff_encoder_bb::sptr diff_encoder_bb::make(unsigned int modulus)
{
    return gnuradio::get_initial_sptr(new diff_encoder_bb_impl(modulus));
}

diff_encoder_bb_impl::diff_encoder_bb_impl(unsigned int modulus)
    : sync_block("diff_encoder_bb",
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 io_signature::make(1, 1, sizeof(unsigned char))),
      d_last_out(0),
      d_modulus(modulus)
{
}

diff_encoder_bb_impl::~diff_encoder_bb_impl() {}

int diff_encoder_bb_impl::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    unsigned last_out = d_last_out;

    for (int i = 0; i < noutput_items; i++) {
        out[i] = (in[i] + last_out) % d_modulus;
        last_out = out[i];
    }

    d_last_out = last_out;
    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
