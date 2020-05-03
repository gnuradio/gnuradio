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

#include "add_const_ii_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

add_const_ii::sptr add_const_ii::make(int k)
{
    return gnuradio::make_block_sptr<add_const_ii_impl>(k);
}

add_const_ii_impl::add_const_ii_impl(int k)
    : sync_block("add_const_ii",
                 io_signature::make(1, 1, sizeof(int)),
                 io_signature::make(1, 1, sizeof(int))),
      d_k(k)
{
}

int add_const_ii_impl::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    const int* iptr = (const int*)input_items[0];
    int* optr = (int*)output_items[0];

    int size = noutput_items;

    while (size >= 8) {
        *optr++ = *iptr++ + d_k;
        *optr++ = *iptr++ + d_k;
        *optr++ = *iptr++ + d_k;
        *optr++ = *iptr++ + d_k;
        *optr++ = *iptr++ + d_k;
        *optr++ = *iptr++ + d_k;
        *optr++ = *iptr++ + d_k;
        *optr++ = *iptr++ + d_k;
        size -= 8;
    }

    while (size-- > 0) {
        *optr++ = *iptr++ + d_k;
    }

    return noutput_items;
}

void add_const_ii_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_get<add_const_ii, int>(
        alias(),
        "Constant",
        &add_const_ii::k,
        pmt::from_long(-2147483648),
        pmt::from_long(2147483647),
        pmt::from_long(0),
        "",
        "Constant to add",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTCPLX | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_set<add_const_ii, int>(alias(),
                                                     "Constant",
                                                     &add_const_ii::set_k,
                                                     pmt::from_long(-2147483648),
                                                     pmt::from_long(2147483647),
                                                     pmt::from_long(0),
                                                     "",
                                                     "Constant to add",
                                                     RPC_PRIVLVL_MIN,
                                                     DISPNULL)));
#endif /* GR_CTRLPORT */
}

} /* namespace blocks */
} /* namespace gr */
