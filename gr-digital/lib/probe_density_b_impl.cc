/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "probe_density_b_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
namespace digital {

probe_density_b::sptr probe_density_b::make(double alpha)
{
    return gnuradio::get_initial_sptr(new probe_density_b_impl(alpha));
}

probe_density_b_impl::probe_density_b_impl(double alpha)
    : sync_block("density_b",
                 io_signature::make(1, 1, sizeof(char)),
                 io_signature::make(0, 0, 0))
{
    set_alpha(alpha);
    d_density = 1.0;
}

probe_density_b_impl::~probe_density_b_impl() {}

void probe_density_b_impl::set_alpha(double alpha)
{
    d_alpha = alpha;
    d_beta = 1.0 - d_alpha;
}

int probe_density_b_impl::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    const char* in = (const char*)input_items[0];

    for (int i = 0; i < noutput_items; i++)
        d_density = d_alpha * (double)in[i] + d_beta * d_density;

    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
