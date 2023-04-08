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

#include "agc_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace analog {

agc_cc::sptr agc_cc::make(float rate, float reference, float gain, float max_gain)
{
    return gnuradio::make_block_sptr<agc_cc_impl>(rate, reference, gain, max_gain);
}

agc_cc_impl::agc_cc_impl(float rate, float reference, float gain, float max_gain)
    : sync_block("agc_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      kernel::agc_cc(rate, reference, gain, max_gain)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));
}

agc_cc_impl::~agc_cc_impl() {}

int agc_cc_impl::work(int noutput_items,
                      gr_vector_const_void_star& input_items,
                      gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];
    scaleN(out, in, noutput_items);
    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */
