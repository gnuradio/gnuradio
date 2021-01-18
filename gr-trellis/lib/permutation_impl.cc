/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "permutation_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>
#include <iostream>

namespace gr {
namespace trellis {

permutation::sptr permutation::make(int K,
                                    const std::vector<int>& TABLE,
                                    int SYMS_PER_BLOCK,
                                    size_t BYTES_PER_SYMBOL)
{
    return gnuradio::make_block_sptr<permutation_impl>(
        K, TABLE, SYMS_PER_BLOCK, BYTES_PER_SYMBOL);
}

permutation_impl::permutation_impl(int K,
                                   const std::vector<int>& TABLE,
                                   int SYMS_PER_BLOCK,
                                   size_t BYTES_PER_SYMBOL)
    : sync_block("permutation",
                 io_signature::make(1, -1, BYTES_PER_SYMBOL),
                 io_signature::make(1, -1, BYTES_PER_SYMBOL)),
      d_K(K),
      d_TABLE(TABLE),
      d_SYMS_PER_BLOCK(SYMS_PER_BLOCK),
      d_BYTES_PER_SYMBOL(BYTES_PER_SYMBOL)
{
    set_output_multiple(d_K * d_SYMS_PER_BLOCK);
    // std::cout << d_K << "\n";
}

void permutation_impl::set_K(int K)
{
    gr::thread::scoped_lock guard(d_setlock);
    d_K = K;
    set_output_multiple(d_K * d_SYMS_PER_BLOCK);
}

void permutation_impl::set_TABLE(const std::vector<int>& table)
{
    gr::thread::scoped_lock guard(d_setlock);
    d_TABLE = table;
}

void permutation_impl::set_SYMS_PER_BLOCK(int spb)
{
    gr::thread::scoped_lock guard(d_setlock);
    d_SYMS_PER_BLOCK = spb;
    set_output_multiple(d_K * d_SYMS_PER_BLOCK);
}

permutation_impl::~permutation_impl() {}

int permutation_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_setlock);
    int nstreams = input_items.size();

    for (int m = 0; m < nstreams; m++) {
        const char* in = (const char*)input_items[m];
        char* out = (char*)output_items[m];

        // per stream processing
        for (int i = 0; i < noutput_items / d_SYMS_PER_BLOCK; i++) {
            // Index i refers to blocks.
            // Beginning of packet (in blocks)
            int i0 = d_K * (i / d_K);
            // position of block within packet (in blocks)
            int j0 = i % d_K;
            // new position of block within packet (in blocks)
            int k0 = d_TABLE[j0];
            memcpy(&(out[i * d_SYMS_PER_BLOCK * d_BYTES_PER_SYMBOL]),
                   &(in[(i0 + k0) * d_SYMS_PER_BLOCK * d_BYTES_PER_SYMBOL]),
                   d_BYTES_PER_SYMBOL * d_SYMS_PER_BLOCK);
        }
        // end per stream processing
    }
    return noutput_items;
}

} /* namespace trellis */
} /* namespace gr */
