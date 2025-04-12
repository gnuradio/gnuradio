/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "hilbert_fc_impl.h"
#include <gnuradio/fft/window.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace filter {

hilbert_fc::sptr
hilbert_fc::make(unsigned int ntaps, fft::window::win_type window, double param)
{
    return gnuradio::make_block_sptr<hilbert_fc_impl>(ntaps, window, param);
}

hilbert_fc_impl::hilbert_fc_impl(unsigned int ntaps,
                                 fft::window::win_type window,
                                 double param)
    : sync_block("hilbert_fc",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_ntaps(ntaps | 0x1), // ensure ntaps is odd
      d_hilb(firdes::hilbert(d_ntaps, window, param))
{
    set_history(d_ntaps);

    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

int hilbert_fc_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        out[i] = gr_complex(in[i + d_ntaps / 2], d_hilb.filter(&in[i]));
    }

    return noutput_items;
}

} /* namespace filter */
} /* namespace gr */
