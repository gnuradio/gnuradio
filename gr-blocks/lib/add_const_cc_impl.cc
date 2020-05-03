/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "add_const_cc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

add_const_cc::sptr add_const_cc::make(gr_complex k)
{
    return gnuradio::make_block_sptr<add_const_cc_impl>(k);
}

add_const_cc_impl::add_const_cc_impl(gr_complex k)
    : sync_block("add_const_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_k(k)
{
}

void add_const_cc_impl::set_k(gr_complex k)
{
    gr::thread::scoped_lock guard(d_setlock);
    d_k = k;
    std::fill(d_k_copy.begin(), d_k_copy.end(), d_k);
}

int add_const_cc_impl::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    const gr_complex* iptr = (const gr_complex*)input_items[0];
    gr_complex* optr = (gr_complex*)output_items[0];

    unsigned long input_size = static_cast<unsigned long>(noutput_items);

    gr::thread::scoped_lock guard(d_setlock);
    if (d_k_copy.size() < input_size) {
        d_k_copy.resize(input_size, d_k);
    }

    volk_32fc_x2_add_32fc(optr, iptr, d_k_copy.data(), noutput_items);

    return noutput_items;
}

void add_const_cc_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_get<add_const_cc, gr_complex>(
        alias(),
        "Constant",
        &add_const_cc::k,
        pmt::from_complex(-4.29e9, 0),
        pmt::from_complex(4.29e9, 0),
        pmt::from_complex(0, 0),
        "",
        "Constant to add",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTCPLX | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_set<add_const_cc, gr_complex>(alias(),
                                                            "Constant",
                                                            &add_const_cc::set_k,
                                                            pmt::from_complex(-4.29e9, 0),
                                                            pmt::from_complex(4.29e9, 0),
                                                            pmt::from_complex(0, 0),
                                                            "",
                                                            "Constant to add",
                                                            RPC_PRIVLVL_MIN,
                                                            DISPNULL)));
#endif /* GR_CTRLPORT */
}

} /* namespace blocks */
} /* namespace gr */
