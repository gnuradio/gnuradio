/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "short_to_char_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

short_to_char::sptr short_to_char::make(size_t vlen)
{
    return gnuradio::make_block_sptr<short_to_char_impl>(vlen);
}

short_to_char_impl::short_to_char_impl(size_t vlen)
    : sync_block("short_to_char",
                 io_signature::make(1, 1, sizeof(short) * vlen),
                 io_signature::make(1, 1, sizeof(char) * vlen)),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(char);
    set_alignment(std::max(1, alignment_multiple));
}

int short_to_char_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    const int16_t* in = (const int16_t*)input_items[0];
    int8_t* out = (int8_t*)output_items[0];

    volk_16i_convert_8i(out, in, d_vlen * noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
