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

#include "constellation_encoder_bc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace digital {

constellation_encoder_bc::sptr
constellation_encoder_bc::make(constellation_sptr constellation)
{
    return gnuradio::make_block_sptr<constellation_encoder_bc_impl>(constellation);
}

constellation_encoder_bc_impl::constellation_encoder_bc_impl(
    constellation_sptr constellation)
    : sync_interpolator("constellation_encoder_bc",
                        io_signature::make(1, 1, sizeof(unsigned char)),
                        io_signature::make(1, 1, sizeof(gr_complex)),
                        constellation->dimensionality()),
      d_constellation(constellation)
{
}

constellation_encoder_bc_impl::~constellation_encoder_bc_impl() {}

void constellation_encoder_bc_impl::set_constellation(
    constellation_sptr new_constellation)
{
    if (new_constellation->dimensionality() != d_constellation->dimensionality()) {
        d_logger->warn("Attempting to change to a new dimensionality constellation (from "
                       "{} to {}). This may cause buffering issues",
                       d_constellation->dimensionality(),
                       new_constellation->dimensionality());
    }

    gr::thread::scoped_lock l(d_mutex);

    d_constellation = new_constellation;
    set_interpolation((uint64_t)d_constellation->dimensionality());
}

int constellation_encoder_bc_impl::work(int noutput_items,
                                        gr_vector_const_void_star& input_items,
                                        gr_vector_void_star& output_items)
{
    unsigned char const* in = (const unsigned char*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    gr::thread::scoped_lock l(d_mutex);

    int ninput_items = noutput_items / d_constellation->dimensionality();

    for (int i = 0; i < ninput_items; i++) {
        d_constellation->map_to_points(in[i],
                                       &out[i * d_constellation->dimensionality()]);
    }

    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
