/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "annotator_raw_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdexcept>

using namespace pmt;

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

void annotator_raw_impl::add_tag(uint64_t offset, pmt_t key, pmt_t val)
{
    gr::thread::scoped_lock l(d_mutex);

    tag_t tag;
    tag.srcid = pmt::intern(name());
    tag.key = key;
    tag.value = val;
    tag.offset = offset;

    // add our new tag
    d_queued_tags.push_back(tag);
    // make sure our tags are in offset order
    std::sort(d_queued_tags.begin(), d_queued_tags.end(), tag_t::offset_compare);
    // make sure we are not adding an item in the past!
    if (tag.offset > nitems_read(0)) {
        throw std::runtime_error(
            "annotator_raw::add_tag: item added too far in the past.");
    }
}

int annotator_raw_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(d_mutex);

    const char* in = (const char*)input_items[0];
    char* out = (char*)output_items[0];

    uint64_t start_N = nitems_read(0);
    uint64_t end_N = start_N + (uint64_t)(noutput_items);

    // locate queued tags that fall in this range and insert them when appropriate
    std::vector<tag_t>::iterator i = d_queued_tags.begin();
    while (i != d_queued_tags.end()) {
        if ((*i).offset >= start_N && (*i).offset < end_N) {
            add_item_tag(0, (*i).offset, (*i).key, (*i).value, (*i).srcid);
            i = d_queued_tags.erase(i);
        } else {
            break;
        }
    }

    // copy data across
    memcpy(out, in, noutput_items * d_itemsize);
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
