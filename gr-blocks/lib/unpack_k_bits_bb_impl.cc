/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "unpack_k_bits_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace blocks {

unpack_k_bits_bb::sptr unpack_k_bits_bb::make(unsigned k)
{
    return gnuradio::make_block_sptr<unpack_k_bits_bb_impl>(k);
}

unpack_k_bits_bb_impl::unpack_k_bits_bb_impl(unsigned k)
    : sync_interpolator("unpack_k_bits_bb",
                        io_signature::make(1, 1, sizeof(unsigned char)),
                        io_signature::make(1, 1, sizeof(unsigned char)),
                        k),
      d_unpack(k)
{
}

int unpack_k_bits_bb_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    d_unpack.unpack(out, in, noutput_items / d_unpack.k());

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
