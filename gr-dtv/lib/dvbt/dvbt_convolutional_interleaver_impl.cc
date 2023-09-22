/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dvbt_convolutional_interleaver_impl.h"
#include <gnuradio/io_signature.h>
#include <deque>

namespace gr {
namespace dtv {

dvbt_convolutional_interleaver::sptr
dvbt_convolutional_interleaver::make(int nsize, int I, int M)
{
    return gnuradio::make_block_sptr<dvbt_convolutional_interleaver_impl>(nsize, I, M);
}

/*
 * The private constructor
 */
dvbt_convolutional_interleaver_impl::dvbt_convolutional_interleaver_impl(int blocks,
                                                                         int I,
                                                                         int M)
    : sync_interpolator("dvbt_convolutional_interleaver",
                        io_signature::make(1, 1, sizeof(unsigned char) * I * blocks),
                        io_signature::make(1, 1, sizeof(unsigned char)),
                        I * blocks),
      d_I(I),
      d_M(M)
{
    // Positions are shift registers (FIFOs)
    // of length i*M
    d_shift.reserve(d_I);
    for (int i = 0; i < d_I; i++) {
        d_shift.emplace_back(d_M * i, 0);
    }
}

/*
 * Our virtual destructor.
 */
dvbt_convolutional_interleaver_impl::~dvbt_convolutional_interleaver_impl() {}

int dvbt_convolutional_interleaver_impl::work(int noutput_items,
                                              gr_vector_const_void_star& input_items,
                                              gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    for (int i = 0; i < (noutput_items / d_I); i++) {
        // Process one block of I symbols
        for (unsigned int j = 0; j < d_shift.size(); j++) {
            d_shift[j].push_front(in[(d_I * i) + j]);
            out[(d_I * i) + j] = d_shift[j].back();
            d_shift[j].pop_back();
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */
