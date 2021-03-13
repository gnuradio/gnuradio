/* -*- c++ -*- */
/*
 * Copyright 2005,2007,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "skiphead_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>

namespace gr {
namespace blocks {

skiphead::sptr skiphead::make(size_t itemsize, uint64_t nitems_to_skip)
{
    return gnuradio::make_block_sptr<skiphead_impl>(itemsize, nitems_to_skip);
}

skiphead_impl::skiphead_impl(size_t itemsize, uint64_t nitems_to_skip)
    : block("skiphead",
            io_signature::make(1, 1, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_nitems_to_skip(nitems_to_skip),
      d_nitems(0)
{
    // Reserve space for a few tags to avoid constant re-allocation
    // in the call to get_tags_in_window
    d_tags.reserve(8);

    // We'll handle propagating tags our selves to handle shifting offsets of tags
    set_tag_propagation_policy(TPP_DONT);
}

skiphead_impl::~skiphead_impl() {}

int skiphead_impl::general_work(int noutput_items,
                                gr_vector_int& ninput_items_,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const char* in = (const char*)input_items[0];
    char* out = (char*)output_items[0];

    int ninput_items = std::min(ninput_items_[0], noutput_items);
    int ii = 0; // input index

    while (ii < ninput_items) {
        uint64_t ni_total = ii + d_nitems; // total items processed so far
        if (ni_total < d_nitems_to_skip) { // need to skip some more

            int n_to_skip =
                (int)std::min(d_nitems_to_skip - ni_total, (uint64_t)(ninput_items - ii));
            ii += n_to_skip;
        }

        else { // nothing left to skip. copy away
            // Grab all tags in the window and shift their offsets appropriately
            get_tags_in_window(d_tags, 0, ii, ninput_items);
            for (auto& tag : d_tags) {
                tag.offset -= d_nitems_to_skip;
                add_item_tag(0, tag);
            }
            int n_to_copy = ninput_items - ii;
            if (n_to_copy > 0) {
                size_t itemsize = output_signature()->sizeof_stream_item(0);
                memcpy(out, in + (ii * itemsize), n_to_copy * itemsize);
            }

            d_nitems += ninput_items;
            consume_each(ninput_items);
            return n_to_copy;
        }
    }

    d_nitems += ninput_items;
    consume_each(ninput_items);
    return 0;
}

} /* namespace blocks */
} /* namespace gr */
