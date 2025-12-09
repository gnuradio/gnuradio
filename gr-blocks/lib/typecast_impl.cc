/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 * Copyright 2025 Sam Lane
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "typecast_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

typecast::sptr typecast::make(size_t sizeof_in_stream_item, size_t sizeof_out_stream_item)
{
    return gnuradio::make_block_sptr<typecast_impl>(sizeof_in_stream_item,
                                                    sizeof_out_stream_item);
}

typecast_impl::typecast_impl(size_t sizeof_in_stream_item, size_t sizeof_out_stream_item)
    : block("typecast",
            io_signature::make(1, 1, sizeof_in_stream_item),
            io_signature::make(1, 1, sizeof_out_stream_item)),
      d_sizeof_in_stream_item(sizeof_in_stream_item),
      d_sizeof_out_stream_item(sizeof_out_stream_item)
{
    if (sizeof_in_stream_item != 1 and sizeof_out_stream_item != 1) {
        throw std::runtime_error(
            "typecasting requires at least one of its streams to be bytes");
    }
    d_copy_modulo = std::max(sizeof_in_stream_item, sizeof_out_stream_item);
}

typecast_impl::~typecast_impl() {}

int typecast_impl::general_work(int noutput_items,
                                gr_vector_int& ninput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    // copy as much as we can, making sure we're still aligned with the stream item sizes
    // we dont want to end up copying half of an item.
    // always round down to the smallest number of bytes we can possibly copy
    // one of the source or dest must always be bytes. we can chain if needed.

    const size_t source_bytes = ninput_items[0] * d_sizeof_in_stream_item;
    const size_t dest_bytes = noutput_items * d_sizeof_out_stream_item;
    const size_t min_bytes = std::min(source_bytes, dest_bytes);
    const size_t copy_bytes = d_copy_modulo * (min_bytes / d_copy_modulo);

    const int in_items = copy_bytes / d_sizeof_in_stream_item;
    const int out_items = copy_bytes / d_sizeof_out_stream_item;

    memcpy(output_items[0], input_items[0], copy_bytes);

    consume(0, in_items);
    return out_items;
}

} /* namespace blocks */
} /* namespace gr */
