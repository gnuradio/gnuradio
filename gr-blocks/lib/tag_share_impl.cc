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

#include "tag_share_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

tag_share::sptr
tag_share::make(size_t sizeof_io_item, size_t sizeof_share_item, size_t vlen)
{
    return gnuradio::make_block_sptr<tag_share_impl>(
        sizeof_io_item, sizeof_share_item, vlen);
}

/*
 * The private constructor
 */
tag_share_impl::tag_share_impl(size_t sizeof_io_item,
                               size_t sizeof_share_item,
                               size_t vlen)
    : gr::sync_block(
          "tag_share",
          gr::io_signature::make2(2, 2, sizeof_io_item * vlen, sizeof_share_item * vlen),
          gr::io_signature::make(1, 1, sizeof_io_item * vlen)),
      d_sizeof_io_item(sizeof_io_item),
      d_vlen(vlen)
{
    // This is the entire premise of the block -- to have the GNU Radio runtime
    // propagate all the tags from Input 0 and Input 1 to Output 0.
    set_tag_propagation_policy(TPP_ALL_TO_ALL);
}

/*
 * Our virtual destructor.
 */
tag_share_impl::~tag_share_impl() {}

int tag_share_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    const void* in = (const void*)input_items[0];
    void* out = (void*)output_items[0];

    // Input 0 passes through to Output 0
    memcpy(out, in, d_sizeof_io_item * d_vlen * noutput_items);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
