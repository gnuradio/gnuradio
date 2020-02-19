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

#include "short_to_float_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

short_to_float::sptr short_to_float::make(size_t vlen, float scale)
{
    return gnuradio::get_initial_sptr(new short_to_float_impl(vlen, scale));
}

short_to_float_impl::short_to_float_impl(size_t vlen, float scale)
    : sync_block("short_to_float",
                 io_signature::make(1, 1, sizeof(short) * vlen),
                 io_signature::make(1, 1, sizeof(float) * vlen)),
      d_vlen(vlen),
      d_scale(scale)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

int short_to_float_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const short* in = (const short*)input_items[0];
    float* out = (float*)output_items[0];

    volk_16i_s32f_convert_32f(out, in, d_scale, d_vlen * noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
