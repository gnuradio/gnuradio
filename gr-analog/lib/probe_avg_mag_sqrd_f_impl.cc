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

#include "probe_avg_mag_sqrd_f_impl.h"
#include <gnuradio/io_signature.h>
#include <cmath>

namespace gr {
namespace analog {

probe_avg_mag_sqrd_f::sptr probe_avg_mag_sqrd_f::make(double threshold_db, double alpha)
{
    return gnuradio::make_block_sptr<probe_avg_mag_sqrd_f_impl>(threshold_db, alpha);
}

probe_avg_mag_sqrd_f_impl::probe_avg_mag_sqrd_f_impl(double threshold_db, double alpha)
    : sync_block("probe_avg_mag_sqrd_f",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(0, 0, 0)),
      d_unmuted(false),
      d_level(0),
      d_iir(alpha)
{
    set_threshold(threshold_db);
}

probe_avg_mag_sqrd_f_impl::~probe_avg_mag_sqrd_f_impl() {}

int probe_avg_mag_sqrd_f_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];

    for (int i = 0; i < noutput_items; i++) {
        double mag_sqrd = in[i] * in[i];
        d_iir.filter(mag_sqrd); // computed for side effect: prev_output()
    }

    d_unmuted = d_iir.prev_output() >= d_threshold;
    d_level = d_iir.prev_output();
    return noutput_items;
}

double probe_avg_mag_sqrd_f_impl::threshold() const
{
    return 10 * std::log10(d_threshold);
}

void probe_avg_mag_sqrd_f_impl::set_threshold(double decibels)
{
    // convert to absolute threshold (mag sqrd)
    d_threshold = std::pow(10.0, decibels / 10);
}

void probe_avg_mag_sqrd_f_impl::set_alpha(double alpha) { d_iir.set_taps(alpha); }

void probe_avg_mag_sqrd_f_impl::reset() { d_iir.reset(); }

} /* namespace analog */
} /* namespace gr */
