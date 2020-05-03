/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "simple_squelch_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <cmath>

namespace gr {
namespace analog {

simple_squelch_cc::sptr simple_squelch_cc::make(double threshold_db, double alpha)
{
    return gnuradio::make_block_sptr<simple_squelch_cc_impl>(threshold_db, alpha);
}

simple_squelch_cc_impl::simple_squelch_cc_impl(double threshold_db, double alpha)
    : sync_block("simple_squelch_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_unmuted(false),
      d_iir(alpha)
{
    set_threshold(threshold_db);
}

simple_squelch_cc_impl::~simple_squelch_cc_impl() {}

void simple_squelch_cc_impl::set_threshold(double decibels)
{
    // convert to absolute threshold (mag squared)
    d_threshold = std::pow(10.0, decibels / 10);
}

double simple_squelch_cc_impl::threshold() const { return 10 * log10(d_threshold); }

void simple_squelch_cc_impl::set_alpha(double alpha) { d_iir.set_taps(alpha); }

std::vector<float> simple_squelch_cc_impl::squelch_range() const
{
    std::vector<float> r(3);
    r[0] = -50.0;               // min	FIXME
    r[1] = +50.0;               // max	FIXME
    r[2] = (r[1] - r[0]) / 100; // step size

    return r;
}

int simple_squelch_cc_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        double mag_sqrd = in[i].real() * in[i].real() + in[i].imag() * in[i].imag();
        double f = d_iir.filter(mag_sqrd);
        if (f >= d_threshold)
            out[i] = in[i];
        else
            out[i] = 0;
    }

    d_unmuted = d_iir.prev_output() >= d_threshold;
    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */
