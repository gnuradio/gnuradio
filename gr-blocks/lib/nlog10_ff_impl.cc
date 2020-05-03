/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "nlog10_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <limits>

namespace gr {
namespace blocks {

nlog10_ff::sptr nlog10_ff::make(float n, size_t vlen, float k)
{
    return gnuradio::make_block_sptr<nlog10_ff_impl>(n, vlen, k);
}

nlog10_ff_impl::nlog10_ff_impl(float n, size_t vlen, float k)
    : sync_block("nlog10_ff",
                 io_signature::make(1, 1, sizeof(float) * vlen),
                 io_signature::make(1, 1, sizeof(float) * vlen)),
      d_n_log2_10(n / log2f(10.0f)),
      d_10_k_n(std::pow(10.0f, k / n)),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

int nlog10_ff_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];
    int noi = noutput_items * d_vlen;

    constexpr float float_min = std::numeric_limits<float>::min();

    for (int i = 0; i < noi; i++) {
        out[i] = std::max(in[i], float_min);
    }

    if (d_10_k_n != 1.0f) {
        volk_32f_s32f_multiply_32f(out, out, d_10_k_n, noi);
    }
    volk_32f_log2_32f(out, out, noi);
    volk_32f_s32f_multiply_32f(out, out, d_n_log2_10, noi);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
