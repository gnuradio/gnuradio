/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#include "annotator_raw_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace gr {
namespace blocks {

annotator_raw::sptr annotator_raw::make(size_t sizeof_stream_item)
{
    return gnuradio::make_block_sptr<annotator_raw_impl>(sizeof_stream_item);
}

annotator_raw_impl::annotator_raw_impl(size_t sizeof_stream_item)
    : sync_block("annotator_raw",
                 io_signature::make(1, 1, sizeof_stream_item),
                 io_signature::make(1, 1, sizeof_stream_item)),
      d_itemsize(sizeof_stream_item)
{
    set_tag_propagation_policy(TPP_ONE_TO_ONE);
    set_relative_rate(1, 1);
}

annotator_raw_impl::~annotator_raw_impl() {}

void annotator_raw_impl::add_tag(uint64_t offset, pmt::pmt_t key, pmt::pmt_t val)
{
    gr::thread::scoped_lock l(d_mutex);

    tag_t tag;
    tag.offset = offset;
    tag.key = key;
    tag.value = val;
    tag.srcid = pmt::intern(name());

    // make sure we are not adding an item in the past!
    if (tag.offset < nitems_read(0)) {
        throw std::runtime_error(fmt::format("annotator_raw::add_tag: item added too far "
                                             "in the past at {}; we're already at {}.",
                                             tag.offset,
                                             nitems_read(0)));
    }
    // add our new tag
    d_queued_tags.insert(tag);
}

int annotator_raw_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(d_mutex);

    uint64_t start_N = nitems_read(0);
    uint64_t end_N = start_N + (uint64_t)(noutput_items);

    // locate queued tags that fall in this range and insert them when appropriate
    const auto lower_bound = d_queued_tags.lower_bound(start_N);
    if (lower_bound != d_queued_tags.end()) {
        // at least one element in range
        const auto upper_bound = d_queued_tags.upper_bound(end_N);
        for (auto iterator = lower_bound; iterator != upper_bound; ++iterator) {
            add_item_tag(0, *iterator);
        }
        if (lower_bound != d_queued_tags.begin()) {
            d_logger->error("dropping in-past tags");
        }
        d_queued_tags.erase(d_queued_tags.begin(), upper_bound);
    }


    // copy data across
    memcpy(output_items[0], input_items[0], noutput_items * d_itemsize);
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
