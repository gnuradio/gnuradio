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

#include "magphase_to_complex_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

magphase_to_complex::sptr magphase_to_complex::make(size_t vlen)
{
    return gnuradio::make_block_sptr<magphase_to_complex_impl>(vlen);
}

magphase_to_complex_impl::magphase_to_complex_impl(size_t vlen)
    : sync_block("magphase_to_complex",
                 io_signature::make(2, 2, sizeof(float) * vlen),
                 io_signature::make(1, 1, sizeof(gr_complex) * vlen)),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

int magphase_to_complex_impl::work(int noutput_items,
                                   gr_vector_const_void_star& input_items,
                                   gr_vector_void_star& output_items)
{
    const float* mag = (const float*)input_items[0];
    const float* phase = (const float*)input_items[1];
    gr_complex* out = (gr_complex*)output_items[0];

    for (size_t j = 0; j < noutput_items * d_vlen; j++)
        out[j] = gr_complex(mag[j] * cos(phase[j]), mag[j] * sin(phase[j]));

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
