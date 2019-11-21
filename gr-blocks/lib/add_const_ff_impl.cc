/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "add_const_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

add_const_ff::sptr add_const_ff::make(float k)
{
    return gnuradio::get_initial_sptr(new add_const_ff_impl(k));
}

add_const_ff_impl::add_const_ff_impl(float k)
    : sync_block("add_const_ff",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float))),
      d_k(k)
{
}

int add_const_ff_impl::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    const float* iptr = (const float*)input_items[0];
    float* optr = (float*)output_items[0];

    int size = noutput_items;

    // build column vec. d_k * ones(size, 1) for volk addition
    unsigned int alignment = volk_get_alignment();
    float * k_copy_vec = (float*)volk_malloc(sizeof(float)*size, alignment);
    for(int ii = 0; ii < size; ii++) {
        k_copy_vec[ii] = d_k;
    }

    volk_32f_x2_add_32f(optr, iptr, k_copy_vec, size);

    volk_free(k_copy_vec);
    return noutput_items;
}

void add_const_ff_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_get<add_const_ff, float>(
        alias(),
        "Constant",
        &add_const_ff::k,
        pmt::from_double(-4.29e9),
        pmt::from_double(4.29e9),
        pmt::from_double(0),
        "",
        "Constant to add",
        RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTCPLX | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_set<add_const_ff, float>(alias(),
                                                       "Constant",
                                                       &add_const_ff::set_k,
                                                       pmt::from_double(-4.29e9),
                                                       pmt::from_double(4.29e9),
                                                       pmt::from_double(0),
                                                       "",
                                                       "Constant to add",
                                                       RPC_PRIVLVL_MIN,
                                                       DISPNULL)));
#endif /* GR_CTRLPORT */
}

} /* namespace blocks */
} /* namespace gr */
