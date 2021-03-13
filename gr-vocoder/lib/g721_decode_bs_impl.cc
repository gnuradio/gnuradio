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

#include "g721_decode_bs_impl.h"
#include <gnuradio/io_signature.h>
#include <climits>

namespace gr {
namespace vocoder {

g721_decode_bs::sptr g721_decode_bs::make()
{
    return gnuradio::make_block_sptr<g721_decode_bs_impl>();
}

g721_decode_bs_impl::g721_decode_bs_impl()
    : sync_block("vocoder_g721_decode_bs",
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 io_signature::make(1, 1, sizeof(short)))
{
    g72x_init_state(&d_state);
}

g721_decode_bs_impl::~g721_decode_bs_impl() {}

int g721_decode_bs_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    short* out = (short*)output_items[0];

    for (int i = 0; i < noutput_items; i++)
        out[i] = g721_decoder(in[i], AUDIO_ENCODING_LINEAR, &d_state);

    return noutput_items;
}

} /* namespace vocoder */
} /* namespace gr */
