/* -*- c++ -*- */
/*
 * Copyright 2016,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "catv_randomizer_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

catv_randomizer_bb::sptr catv_randomizer_bb::make(catv_constellation_t constellation)
{
    return gnuradio::make_block_sptr<catv_randomizer_bb_impl>(constellation);
}

/*
 * The private constructor
 */
catv_randomizer_bb_impl::catv_randomizer_bb_impl(catv_constellation_t constellation)
    : gr::sync_block("catv_randomizer_bb",
                     gr::io_signature::make(1, 1, sizeof(unsigned char)),
                     gr::io_signature::make(1, 1, sizeof(unsigned char)))
{
    offset = 0;
    if (constellation == CATV_MOD_64QAM) {
        max_offset = 60 * 128;
    } else {
        max_offset = 88 * 128;
    }
    init_rand();
}

/*
 * Our virtual destructor.
 */
catv_randomizer_bb_impl::~catv_randomizer_bb_impl() {}

void catv_randomizer_bb_impl::init_rand()
{
    unsigned char c2 = 0x7F, c1 = 0x7F, c0 = 0x7F;
    unsigned char c2_new, c1_new, c0_new;
    int n, i;

    for (n = 0; n < max_offset; n++) {
        rseq[n] = c2;
        c2_new = c1;
        c1_new = c0 ^ c2;
        c0_new = c2;
        for (i = 0; i < 3; i++) {
            c0_new <<= 1;
            if (c0_new & 0x80) {
                c0_new = (c0_new & 0x7F) ^ 0x09;
            }
        }
        c2 = c2_new;
        c1 = c1_new;
        c0 = c0_new;
    }
}

int catv_randomizer_bb_impl::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        out[i] = in[i] ^ rseq[offset++];
        if (offset == max_offset) {
            offset = 0;
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */
