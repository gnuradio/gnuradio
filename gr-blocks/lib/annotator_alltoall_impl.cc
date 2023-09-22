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

#include "annotator_alltoall_impl.h"
#include <gnuradio/io_signature.h>
#include <sstream>

namespace gr {
namespace blocks {

annotator_alltoall::sptr annotator_alltoall::make(int when, size_t sizeof_stream_item)
{
    return gnuradio::make_block_sptr<annotator_alltoall_impl>(when, sizeof_stream_item);
}

annotator_alltoall_impl::annotator_alltoall_impl(int when, size_t sizeof_stream_item)
    : sync_block("annotator_alltoall",
                 io_signature::make(1, -1, sizeof_stream_item),
                 io_signature::make(1, -1, sizeof_stream_item)),
      d_when((uint64_t)when)
{
    set_tag_propagation_policy(TPP_ALL_TO_ALL);

    d_tag_counter = 0;
}

annotator_alltoall_impl::~annotator_alltoall_impl() {}

int annotator_alltoall_impl::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];

    std::stringstream str;
    str << name() << unique_id();

    uint64_t abs_N = 0, end_N;
    int ninputs = input_items.size();
    for (int i = 0; i < ninputs; i++) {
        abs_N = nitems_read(i);
        end_N = abs_N + (uint64_t)(noutput_items);

        std::vector<tag_t> all_tags;
        get_tags_in_range(all_tags, i, abs_N, end_N);
        d_stored_tags.insert(d_stored_tags.end(), all_tags.begin(), all_tags.end());
    }

    // Source ID and key for any tag that might get applied from this block
    pmt::pmt_t srcid = pmt::string_to_symbol(str.str());
    static pmt::pmt_t key = pmt::string_to_symbol("seq");

    // Work does nothing to the data stream; just copy all inputs to
    // outputs Adds a new tag when the number of items read is a
    // multiple of d_when
    abs_N = nitems_written(0);
    int noutputs = output_items.size();

    for (int j = 0; j < noutput_items; j++) {
        for (int i = 0; i < noutputs; i++) {
            if (abs_N % d_when == 0) {
                pmt::pmt_t value = pmt::from_uint64(d_tag_counter++);
                add_item_tag(i, abs_N, key, value, srcid);
            }

            // Sum all of the inputs together for each output. Just 'cause.
            out = (float*)output_items[i];
            out[j] = 0;
            for (int ins = 0; ins < ninputs; ins++) {
                in = (const float*)input_items[ins];
                out[j] += in[j];
            }
        }
        abs_N++;
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
