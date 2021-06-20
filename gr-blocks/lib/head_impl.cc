/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2013 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "head_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>

namespace gr {
namespace blocks {

head::sptr head::make(size_t sizeof_stream_item, uint64_t nitems)
{
    return gnuradio::make_block_sptr<head_impl>(sizeof_stream_item, nitems);
}

head_impl::head_impl(size_t sizeof_stream_item, uint64_t nitems)
    : sync_block("head",
                 io_signature::make(1, 1, sizeof_stream_item),
                 io_signature::make(0, 1, sizeof_stream_item)),
      d_nitems(nitems),
      d_ncopied_items(0)
{
}

head_impl::~head_impl() {}

int head_impl::work(int noutput_items,
                    gr_vector_const_void_star& input_items,
                    gr_vector_void_star& output_items)
{
    if (d_ncopied_items >= d_nitems)
        return -1; // Done!

    unsigned n = std::min(d_nitems - d_ncopied_items, (uint64_t)noutput_items);

    if (n == 0)
        return 0;

    // can have zero or one output port, if zero, don't copy
    if (!output_items.empty()) {
        memcpy(output_items[0],
               input_items[0],
               n * input_signature()->sizeof_stream_item(0));
    }
    d_ncopied_items += n;

    return n;
}

} /* namespace blocks */
} /* namespace gr */
