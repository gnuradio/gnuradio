/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "exponentiate_const_cci_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

exponentiate_const_cci::sptr exponentiate_const_cci::make(int exponent, size_t vlen)
{
    return gnuradio::make_block_sptr<exponentiate_const_cci_impl>(exponent, vlen);
}

/*
 * The private constructor
 */
exponentiate_const_cci_impl::exponentiate_const_cci_impl(int exponent, size_t vlen)
    : gr::sync_block("exponentiate_const_cci",
                     gr::io_signature::make(1, -1, sizeof(gr_complex) * vlen),
                     gr::io_signature::make(1, -1, sizeof(gr_complex) * vlen)),
      d_exponent(exponent),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));
}

/*
 * Our virtual destructor.
 */
exponentiate_const_cci_impl::~exponentiate_const_cci_impl() {}

bool exponentiate_const_cci_impl::check_topology(int ninputs, int noutputs)
{
    return ninputs == noutputs;
}

void exponentiate_const_cci_impl::set_exponent(int exponent)
{
    gr::thread::scoped_lock guard(d_setlock);
    d_exponent = exponent;
}

int exponentiate_const_cci_impl::work(int noutput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_setlock);

    for (unsigned int i = 0; i < input_items.size(); i++) {
        const gr_complex* in = (const gr_complex*)input_items[i];
        gr_complex* out = (gr_complex*)output_items[i];

        if (d_exponent > 1) {
            volk_32fc_x2_multiply_32fc(out, in, in, noutput_items * d_vlen);
            for (int j = 2; j < d_exponent; j++) {
                volk_32fc_x2_multiply_32fc(out, out, in, noutput_items * d_vlen);
            }
        } else {
            memcpy(out, in, sizeof(gr_complex) * noutput_items * d_vlen);
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
