/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "conjugate_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

conjugate_cc::sptr conjugate_cc::make()
{
    return gnuradio::make_block_sptr<conjugate_cc_impl>();
}

conjugate_cc_impl::conjugate_cc_impl()
    : sync_block("conjugate_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex)))
{
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));
}

int conjugate_cc_impl::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    const gr_complex* iptr = (const gr_complex*)input_items[0];
    gr_complex* optr = (gr_complex*)output_items[0];

    volk_32fc_conjugate_32fc(optr, iptr, noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
