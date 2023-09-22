/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "g723_40_encode_sb_impl.h"
#include <gnuradio/io_signature.h>
#include <climits>

namespace gr {
namespace vocoder {

g723_40_encode_sb::sptr g723_40_encode_sb::make()
{
    return gnuradio::make_block_sptr<g723_40_encode_sb_impl>();
}

g723_40_encode_sb_impl::g723_40_encode_sb_impl()
    : sync_block("vocoder_g723_40_encode_sb",
                 io_signature::make(1, 1, sizeof(short)),
                 io_signature::make(1, 1, sizeof(unsigned char)))
{
    g72x_init_state(&d_state);
}

g723_40_encode_sb_impl::~g723_40_encode_sb_impl() {}

int g723_40_encode_sb_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const short* in = (const short*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    for (int i = 0; i < noutput_items; i++)
        out[i] = g723_40_encoder(in[i], AUDIO_ENCODING_LINEAR, &d_state);

    return noutput_items;
}

} /* namespace vocoder */
} /* namespace gr */
