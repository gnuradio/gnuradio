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

#include "interleaved_char_to_complex_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

interleaved_char_to_complex::sptr interleaved_char_to_complex::make(bool vector_input,
                                                                    float scale_factor)
{
    return gnuradio::make_block_sptr<interleaved_char_to_complex_impl>(vector_input,
                                                                       scale_factor);
}

interleaved_char_to_complex_impl::interleaved_char_to_complex_impl(bool vector_input,
                                                                   float scale_factor)
    : sync_decimator("interleaved_char_to_complex",
                     gr::io_signature::make(1, 1, (vector_input ? 2 : 1) * sizeof(char)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     vector_input ? 1 : 2),
      d_scalar(scale_factor)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));
}

int interleaved_char_to_complex_impl::work(int noutput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    float* out = (float*)output_items[0];
    const int8_t* in = (const int8_t*)input_items[0];

    // This calculates in[] * 1.0 / d_scalar
    volk_8i_s32f_convert_32f(out, in, d_scalar, 2 * noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
