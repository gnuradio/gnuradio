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

#include "interleaved_short_to_complex_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

interleaved_short_to_complex::sptr
interleaved_short_to_complex::make(bool vector_input, bool swap, float scale_factor)
{
    return gnuradio::make_block_sptr<interleaved_short_to_complex_impl>(
        vector_input, swap, scale_factor);
}

interleaved_short_to_complex_impl::interleaved_short_to_complex_impl(bool vector_input,
                                                                     bool swap,
                                                                     float scale_factor)
    : sync_decimator("interleaved_short_to_complex",
                     gr::io_signature::make(1, 1, (vector_input ? 2 : 1) * sizeof(short)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     vector_input ? 1 : 2),
      d_scalar(scale_factor),
      d_swap(swap)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));
}

void interleaved_short_to_complex_impl::set_swap(bool swap) { d_swap = swap; }

int interleaved_short_to_complex_impl::work(int noutput_items,
                                            gr_vector_const_void_star& input_items,
                                            gr_vector_void_star& output_items)
{
    const short* in = (const short*)input_items[0];
    float* out = (float*)output_items[0];

    // This calculates in[] * 1.0 / d_scalar
    volk_16i_s32f_convert_32f(out, in, d_scalar, 2 * noutput_items);

    if (d_swap) {
        float* p = (float*)output_items[0];
        for (int i = 0; i < noutput_items; ++i) {
            float f = p[2 * i + 1];
            p[2 * i + 1] = p[2 * i];
            p[2 * i] = f;
        }
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
