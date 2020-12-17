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

#include "keep_one_in_n_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

keep_one_in_n::sptr keep_one_in_n::make(size_t itemsize, int n)
{
    return gnuradio::make_block_sptr<keep_one_in_n_impl>(itemsize, n);
}

keep_one_in_n_impl::keep_one_in_n_impl(size_t itemsize, int n)
    : block("keep_one_in_n",
            io_signature::make(1, 1, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_count(n)
{
    // To avoid bad behavior with using set_relative_rate in this block with
    // VERY large values of n, we will keep track of things ourselves. Using
    // this to turn off automatic tag propagation, which will be handled
    // locally in general_work().
    set_tag_propagation_policy(TPP_DONT);

    set_n(n);
}

void keep_one_in_n_impl::set_n(int n)
{
    if (n < 1) {
        throw std::invalid_argument("N must be at least 1.");
    }

    d_n = n;
    d_count = n;

    // keep our internal understanding of the relative rate of this block
    // don't set the relative rate, though, and we will handle our own
    // tag propagation.
    d_decim_rate = 1.0 / (float)d_n;
}

int keep_one_in_n_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    const char* in = (const char*)input_items[0];
    char* out = (char*)output_items[0];

    size_t item_size = input_signature()->sizeof_stream_item(0);
    int ni = 0;
    int no = 0;

    while (ni < ninput_items[0] && no < noutput_items) {
        d_count--;
        if (d_count <= 0) {
            memcpy(out, in, item_size); // copy 1 item
            out += item_size;
            no++;
            d_count = d_n;
        }
        in += item_size;
        ni++;
    }

    // Because we have set TPP_DONT, we have to propagate the tags here manually.
    // Adjustment of the tag sample value is done using the float d_decim_rate.
    std::vector<tag_t> tags;
    get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ni);
    for (const auto& tag : tags) {
        tag_t new_tag = tag;
        new_tag.offset *= d_decim_rate;
        add_item_tag(0, new_tag);
    }

    consume_each(ni);
    return no;
}

} /* namespace blocks */
} /* namespace gr */
