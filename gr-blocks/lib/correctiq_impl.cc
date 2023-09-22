/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "correctiq_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

correctiq::sptr correctiq::make() { return gnuradio::make_block_sptr<correctiq_impl>(); }

/*
 * The private constructor
 */
correctiq_impl::correctiq_impl()
    : gr::sync_block("correctiq",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_avg_real(0.0),
      d_avg_img(0.0),
      d_ratio(1e-05f)
{
}

/*
 * Our virtual destructor.
 */
correctiq_impl::~correctiq_impl() {}

int correctiq_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        d_avg_real = d_ratio * (in[i].real() - d_avg_real) + d_avg_real;
        d_avg_img = d_ratio * (in[i].imag() - d_avg_img) + d_avg_img;

        out[i].real(in[i].real() - d_avg_real);
        out[i].imag(in[i].imag() - d_avg_img);
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
