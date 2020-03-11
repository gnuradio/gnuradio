/* -*- c++ -*- */
/*
 * Copyright 2012-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "pack_k_bits_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>
#include <stdexcept>

namespace gr {
namespace blocks {

pack_k_bits_bb::sptr pack_k_bits_bb::make(unsigned k)
{
    return gnuradio::get_initial_sptr(new pack_k_bits_bb_impl(k));
}

pack_k_bits_bb_impl::pack_k_bits_bb_impl(unsigned k)
    : sync_decimator("pack_k_bits_bb",
                     io_signature::make(1, 1, sizeof(unsigned char)),
                     io_signature::make(1, 1, sizeof(unsigned char)),
                     k),
      d_pack(k)
{
}

int pack_k_bits_bb_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    d_pack.pack(out, in, noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
