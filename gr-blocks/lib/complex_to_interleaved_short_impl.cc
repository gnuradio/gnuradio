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

#include "complex_to_interleaved_short_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

complex_to_interleaved_short::sptr complex_to_interleaved_short::make(bool vector)
{
    return gnuradio::get_initial_sptr(new complex_to_interleaved_short_impl(vector));
}

complex_to_interleaved_short_impl::complex_to_interleaved_short_impl(bool vector)
    : sync_interpolator(
          "complex_to_interleaved_short",
          io_signature::make(1, 1, sizeof(gr_complex)),
          io_signature::make(1, 1, vector ? 2 * sizeof(short) : sizeof(short)),
          vector ? 1 : 2),
      d_vector(vector)
{
}

int complex_to_interleaved_short_impl::work(int noutput_items,
                                            gr_vector_const_void_star& input_items,
                                            gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    short* out = (short*)output_items[0];

    int npairs = (d_vector ? noutput_items : noutput_items / 2);
    for (int i = 0; i < npairs; i++) {
        *out++ = (short)lrintf(in[i].real()); // FIXME saturate?
        *out++ = (short)lrintf(in[i].imag());
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
