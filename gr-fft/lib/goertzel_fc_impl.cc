/* -*- c++ -*- */
/*
 * Copyright 2006,2010-2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "goertzel_fc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace fft {

goertzel_fc::sptr goertzel_fc::make(int rate, int len, float freq)
{
    return gnuradio::make_block_sptr<goertzel_fc_impl>(rate, len, freq);
}

goertzel_fc_impl::goertzel_fc_impl(int rate, int len, float freq)
    : sync_decimator("goertzel_fc",
                     io_signature::make(1, 1, sizeof(float)),
                     io_signature::make(1, 1, sizeof(gr_complex)),
                     len),
      d_goertzel(rate, len, freq),
      d_len(len),
      d_freq(freq),
      d_rate(rate)
{
}

void goertzel_fc_impl::set_freq(float freq)
{
    d_freq = freq;
    d_goertzel.set_params(d_rate, d_len, d_freq);
}

void goertzel_fc_impl::set_rate(int rate)
{
    d_rate = rate;
    d_goertzel.set_params(d_rate, d_len, d_freq);
}

int goertzel_fc_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    const float* in = static_cast<const float*>(input_items[0]);
    gr_complex* out = (gr_complex*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        *out++ = d_goertzel.batch(in);
        in += d_len;
    }

    return noutput_items;
}

} /* namespace fft */
} /* namespace gr */
