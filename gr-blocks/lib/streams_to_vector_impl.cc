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

#include "streams_to_vector_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

streams_to_vector::sptr streams_to_vector::make(size_t itemsize, size_t nstreams)
{
    return gnuradio::make_block_sptr<streams_to_vector_impl>(itemsize, nstreams);
}

streams_to_vector_impl::streams_to_vector_impl(size_t itemsize, size_t nstreams)
    : sync_block("streams_to_vector",
                 io_signature::make(nstreams, nstreams, itemsize),
                 io_signature::make(1, 1, nstreams * itemsize))
{
}

int streams_to_vector_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    size_t itemsize = input_signature()->sizeof_stream_item(0);
    int nstreams = input_items.size();

    const char** inv = (const char**)&input_items[0];
    char* out = (char*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        for (int j = 0; j < nstreams; j++) {
            memcpy(out, inv[j], itemsize);
            inv[j] += itemsize;
            out += itemsize;
        }
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
