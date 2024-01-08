/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 * Copyright 2024 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "float_array_to_uchar.h"
#include "float_to_uchar_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <cmath>

namespace gr {
namespace blocks {

float_to_uchar::sptr float_to_uchar::make(size_t vlen, float scale, float bias)
{
    return gnuradio::make_block_sptr<float_to_uchar_impl>(vlen, scale, bias);
}

float_to_uchar_impl::float_to_uchar_impl(size_t vlen, float scale, float bias)
    : sync_block("float_to_uchar",
                 io_signature::make(1, 1, sizeof(float) * vlen),
                 io_signature::make(1, 1, sizeof(unsigned char) * vlen)),
      d_vlen(vlen),
      d_scale(scale),
      d_bias(bias)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(char);
    set_alignment(std::max(1, alignment_multiple));
}

int float_to_uchar_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

#if VOLK_VERSION >= 030100
    volk_32f_s32f_x2_convert_8u(out, in, d_scale, d_bias, d_vlen * noutput_items);
#else
    const float min_val = 0.0f;
    const float max_val = UINT8_MAX;
    const int nitems = d_vlen * noutput_items;
    for (int j = 0; j < nitems; ++j) {
        const float r = in[j] * d_scale + d_bias;
        unsigned char u;
        if (r > max_val) {
            u = (uint8_t)(max_val);
        } else if (r < min_val) {
            u = (uint8_t)(min_val);
        } else {
            u = (uint8_t)(rintf(r));
        }
        out[j] = u;
    }
#endif

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
