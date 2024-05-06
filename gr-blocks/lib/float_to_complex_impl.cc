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

#include "float_to_complex_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

float_to_complex::sptr float_to_complex::make(size_t vlen)
{
    return gnuradio::make_block_sptr<float_to_complex_impl>(vlen);
}

float_to_complex_impl::float_to_complex_impl(size_t vlen)
    : sync_block("float_to_complex",
                 io_signature::make(1, 2, sizeof(float) * vlen),
                 io_signature::make(1, 1, sizeof(gr_complex) * vlen)),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

int float_to_complex_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const float* r = (const float*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    switch (input_items.size()) {
    case 1:
        for (size_t j = 0; j < noutput_items * d_vlen; j++)
            out[j] = gr_complex(r[j], 0);
        break;

    case 2: {
        // for (size_t j = 0; j < noutput_items*d_vlen; j++)
        //  out[j] = gr_complex (r[j], i[j]);
        const float* i = (const float*)input_items[1];
        volk_32f_x2_interleave_32fc(out, r, i, noutput_items * d_vlen);
        break;
    }

    default:
        assert(0);
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
