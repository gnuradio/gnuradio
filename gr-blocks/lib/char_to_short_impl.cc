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

#include "char_to_short_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

char_to_short::sptr char_to_short::make(size_t vlen)
{
    return gnuradio::make_block_sptr<char_to_short_impl>(vlen);
}

char_to_short_impl::char_to_short_impl(size_t vlen)
    : sync_block("char_to_short",
                 io_signature::make(1, 1, sizeof(char) * vlen),
                 io_signature::make(1, 1, sizeof(short) * vlen)),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(short);
    set_alignment(std::max(1, alignment_multiple));
}

int char_to_short_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    const int8_t* in = (const int8_t*)input_items[0];
    int16_t* out = (int16_t*)output_items[0];

    volk_8i_convert_16i(out, in, d_vlen * noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
