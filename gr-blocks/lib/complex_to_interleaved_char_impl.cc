/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "complex_to_interleaved_char_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

complex_to_interleaved_char::sptr complex_to_interleaved_char::make(bool vector,
                                                                    float scale_factor)
{
    return gnuradio::make_block_sptr<complex_to_interleaved_char_impl>(vector,
                                                                       scale_factor);
}

complex_to_interleaved_char_impl::complex_to_interleaved_char_impl(bool vector,
                                                                   float scale_factor)
    : sync_interpolator(
          "complex_to_interleaved_char",
          io_signature::make(1, 1, sizeof(gr_complex)),
          io_signature::make(1, 1, vector ? 2 * sizeof(char) : sizeof(char)),
          vector ? 1 : 2),
      d_scalar(scale_factor),
      d_vector(vector)
{
}

int complex_to_interleaved_char_impl::work(int noutput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    int8_t* out = (int8_t*)output_items[0];

    volk_32f_s32f_convert_8i(
        out, in, d_scalar, d_vector ? noutput_items * 2 : noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
