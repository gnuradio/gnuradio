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

#include "ulaw_decode_bs_impl.h"
#include <gnuradio/io_signature.h>
#include <climits>

namespace gr {
namespace vocoder {

extern "C" {
#include "g7xx/g72x.h"
}

ulaw_decode_bs::sptr ulaw_decode_bs::make()
{
    return gnuradio::make_block_sptr<ulaw_decode_bs_impl>();
}

ulaw_decode_bs_impl::ulaw_decode_bs_impl()
    : sync_block("vocoder_ulaw_decode_bs",
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 io_signature::make(1, 1, sizeof(short)))
{
}

ulaw_decode_bs_impl::~ulaw_decode_bs_impl() {}

int ulaw_decode_bs_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    short* out = (short*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        out[i] = ulaw2linear(in[i]);
    }

    return noutput_items;
}

} /* namespace vocoder */
} /* namespace gr */
