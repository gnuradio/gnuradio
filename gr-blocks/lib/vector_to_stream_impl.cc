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

#include "vector_to_stream_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

vector_to_stream::sptr vector_to_stream::make(size_t itemsize, size_t nitems_per_block)
{
    return gnuradio::make_block_sptr<vector_to_stream_impl>(itemsize, nitems_per_block);
}

vector_to_stream_impl::vector_to_stream_impl(size_t itemsize, size_t nitems_per_block)
    : sync_interpolator("vector_to_stream",
                        io_signature::make(1, 1, itemsize * nitems_per_block),
                        io_signature::make(1, 1, itemsize),
                        nitems_per_block)
{
}

int vector_to_stream_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    size_t block_size = output_signature()->sizeof_stream_item(0);

    const char* in = (const char*)input_items[0];
    char* out = (char*)output_items[0];

    memcpy(out, in, noutput_items * block_size);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
