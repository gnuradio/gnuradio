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

#include "annotator_1to1_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>
#include <sstream>

namespace gr {
namespace blocks {

annotator_1to1::sptr annotator_1to1::make(int when, size_t sizeof_stream_item)
{
    return gnuradio::make_block_sptr<annotator_1to1_impl>(when, sizeof_stream_item);
}

annotator_1to1_impl::annotator_1to1_impl(int when, size_t sizeof_stream_item)
    : sync_block("annotator_1to1",
                 io_signature::make(1, -1, sizeof_stream_item),
                 io_signature::make(1, -1, sizeof_stream_item)),
      d_when((uint64_t)when)
{
    set_tag_propagation_policy(TPP_ONE_TO_ONE);

    d_tag_counter = 0;
    set_relative_rate(1, 1);
}

annotator_1to1_impl::~annotator_1to1_impl() {}

int annotator_1to1_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];

    std::stringstream str;
    str << name() << unique_id();

    uint64_t abs_N = 0;
    int ninputs = input_items.size();
    for (int i = 0; i < ninputs; i++) {
        abs_N = nitems_read(i);

        std::vector<tag_t> all_tags;
        get_tags_in_range(all_tags, i, abs_N, abs_N + noutput_items);
        d_stored_tags.insert(d_stored_tags.end(), all_tags.begin(), all_tags.end());
    }

    // Storing the current noutput_items as the value to the "noutput_items" key
    pmt::pmt_t srcid = pmt::string_to_symbol(str.str());
    pmt::pmt_t key = pmt::string_to_symbol("seq");

    // Work does nothing to the data stream; just copy all inputs to outputs
    // Adds a new tag when the number of items read is a multiple of d_when
    abs_N = nitems_read(0);
    int noutputs = output_items.size();
    for (int j = 0; j < noutput_items; j++) {
        // the min() is a hack to make sure this doesn't segfault if
        // there are a different number of ins and outs. This is
        // specifically designed to test the 1-to-1 propagation policy.
        for (int i = 0; i < std::min(noutputs, ninputs); i++) {
            if (abs_N % d_when == 0) {
                pmt::pmt_t value = pmt::from_uint64(d_tag_counter++);
                add_item_tag(i, abs_N, key, value, srcid);
            }

            in = (const float*)input_items[i];
            out = (float*)output_items[i];
            out[j] = in[j];
        }
        abs_N++;
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
