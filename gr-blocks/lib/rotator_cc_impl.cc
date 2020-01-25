/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rotator_cc_impl.h"
#include <gnuradio/io_signature.h>

#include <cmath>

namespace gr {
namespace blocks {

rotator_cc::sptr rotator_cc::make(double phase_inc)
{
    return gnuradio::get_initial_sptr(new rotator_cc_impl(phase_inc));
}

rotator_cc_impl::rotator_cc_impl(double phase_inc)
    : sync_block("rotator_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex)))
{
    set_phase_inc(phase_inc);
}

rotator_cc_impl::~rotator_cc_impl() {}

void rotator_cc_impl::set_phase_inc(double phase_inc)
{
    d_r.set_phase_incr(exp(gr_complex(0, phase_inc)));
}

int rotator_cc_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

#if 0
      for (int i=0; i<noutput_items; i++)
      	out[i] = d_r.rotate(in[i]);
#else
    d_r.rotateN(out, in, noutput_items);
#endif

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
