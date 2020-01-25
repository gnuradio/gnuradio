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

interleaved_char_to_complex::sptr interleaved_char_to_complex::make(bool vector_input)
{
    return gnuradio::get_initial_sptr(new interleaved_char_to_complex_impl(vector_input));
}

interleaved_char_to_complex_impl::interleaved_char_to_complex_impl(bool vector_input)
    : sync_decimator("interleaved_char_to_complex",
                     gr::io_signature::make(1, 1, (vector_input ? 2 : 1) * sizeof(char)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     vector_input ? 1 : 2)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));
}

int interleaved_char_to_complex_impl::work(int noutput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    const int8_t* in = (const int8_t*)input_items[0];
    float* out = (float*)output_items[0];

    volk_8i_s32f_convert_32f_u(out, in, 1.0, 2 * noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
