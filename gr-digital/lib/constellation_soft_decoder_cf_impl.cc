/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "constellation_soft_decoder_cf_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace digital {

constellation_soft_decoder_cf::sptr
constellation_soft_decoder_cf::make(constellation_sptr constellation, float npwr)
{
    return gnuradio::make_block_sptr<constellation_soft_decoder_cf_impl>(constellation,
                                                                         npwr);
}

constellation_soft_decoder_cf_impl::constellation_soft_decoder_cf_impl(
    constellation_sptr constellation, float npwr)
    : sync_interpolator("constellation_soft_decoder_cf",
                        io_signature::make(1, 1, sizeof(gr_complex)),
                        io_signature::make(1, 1, sizeof(float)),
                        constellation->bits_per_symbol()),
      d_constellation(constellation),
      d_dim(constellation->dimensionality()),
      d_bps(constellation->bits_per_symbol()),
      d_npwr(npwr)
{
    if (d_npwr > 0) {
        d_constellation->set_npwr(d_npwr);
    }
}

void constellation_soft_decoder_cf_impl::set_npwr(float npwr)
{
    d_npwr = npwr;
    d_constellation->set_npwr(npwr);
}
constellation_soft_decoder_cf_impl::~constellation_soft_decoder_cf_impl() {}


void constellation_soft_decoder_cf_impl::set_constellation(
    constellation_sptr new_constellation)
{
    if (new_constellation->dimensionality() != d_dim) {
        d_logger->warn("Attempting to change to a new dimensionality constellation (from "
                       "{} to {}). This may cause buffering issues.",
                       d_dim,
                       new_constellation->dimensionality());
    }
    if (new_constellation->bits_per_symbol() != d_bps) {
        if (!d_warned_bps) {
            d_logger->warn(
                "Attempting to change to a constellation with different number of "
                "bits per symbol (from {} to {}). This may cause buffering issues. This "
                "warning is raised only once.",
                d_bps,
                new_constellation->bits_per_symbol());
            d_warned_bps = true;
        }
    }

    gr::thread::scoped_lock l(d_mutex);

    d_constellation = new_constellation;
    d_dim = d_constellation->dimensionality();
    d_bps = d_constellation->bits_per_symbol();
    set_interpolation(d_bps);
    // set_relative_rate((uint64_t)d_dim, (uint64_t)d_bps); // For when d_dim is properly
    // managed
}

int constellation_soft_decoder_cf_impl::work(int noutput_items,
                                             gr_vector_const_void_star& input_items,
                                             gr_vector_void_star& output_items)
{
    gr_complex const* in = (const gr_complex*)input_items[0];
    float* out = (float*)output_items[0];

    std::vector<float> bits;

    gr::thread::scoped_lock l(d_mutex);

    // FIXME: figure out how to manage d_dim
    for (unsigned int i = 0; i < noutput_items / d_bps; i++) {
        bits = d_constellation->soft_decision_maker(in[i]);
        for (size_t j = 0; j < bits.size(); j++) {
            out[d_bps * i + j] = bits[j];
        }
    }

    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
