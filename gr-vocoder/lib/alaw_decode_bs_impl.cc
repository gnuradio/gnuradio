/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "alaw_decode_bs_impl.h"
#include <gnuradio/io_signature.h>
#include <climits>

namespace gr {
namespace vocoder {

extern "C" {
#include "g7xx/g72x.h"
}

alaw_decode_bs::sptr alaw_decode_bs::make()
{
    return gnuradio::make_block_sptr<alaw_decode_bs_impl>();
}

alaw_decode_bs_impl::alaw_decode_bs_impl()
    : sync_block("vocoder_alaw_decode_bs",
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 io_signature::make(1, 1, sizeof(short)))
{
}

alaw_decode_bs_impl::~alaw_decode_bs_impl() {}

int alaw_decode_bs_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    short* out = (short*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        out[i] = alaw2linear(in[i]);
    }

    return noutput_items;
}

} /* namespace vocoder */
} /* namespace gr */
