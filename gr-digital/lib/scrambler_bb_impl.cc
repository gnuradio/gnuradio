/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "scrambler_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace digital {

scrambler_bb::sptr scrambler_bb::make(uint64_t mask, uint64_t seed, uint8_t len)
{
    return gnuradio::make_block_sptr<scrambler_bb_impl>(mask, seed, len);
}

scrambler_bb_impl::scrambler_bb_impl(uint64_t mask, uint64_t seed, uint8_t len)
    : sync_block("scrambler_bb",
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 io_signature::make(1, 1, sizeof(unsigned char))),
      d_lfsr(mask, seed, len)
{
}

scrambler_bb_impl::~scrambler_bb_impl() {}

int scrambler_bb_impl::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        out[i] = d_lfsr.next_bit_scramble(in[i]);
    }

    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
