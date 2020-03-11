/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "diff_phasor_cc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace digital {

diff_phasor_cc::sptr diff_phasor_cc::make()
{
    return gnuradio::get_initial_sptr(new diff_phasor_cc_impl());
}

diff_phasor_cc_impl::diff_phasor_cc_impl()
    : sync_block("diff_phasor_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex)))
{
    set_history(2);
}

diff_phasor_cc_impl::~diff_phasor_cc_impl() {}

int diff_phasor_cc_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    gr_complex const* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];
    in += 1; // ensure that i - 1 is valid.

    for (int i = 0; i < noutput_items; i++) {
        out[i] = in[i] * conj(in[i - 1]);
    }

    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
