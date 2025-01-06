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

#include "tagged_stream_mux_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

tagged_stream_mux::sptr tagged_stream_mux::make(size_t itemsize,
                                                const std::string& lengthtagname,
                                                unsigned int tag_preserve_head_pos)
{
    return gnuradio::make_block_sptr<tagged_stream_mux_impl>(
        itemsize, lengthtagname, tag_preserve_head_pos);
}

tagged_stream_mux_impl::tagged_stream_mux_impl(size_t itemsize,
                                               const std::string& lengthtagname,
                                               unsigned int tag_preserve_head_pos)
    : tagged_stream_block("tagged_stream_mux",
                          io_signature::make(1, -1, itemsize),
                          io_signature::make(1, 1, itemsize),
                          lengthtagname),
      d_itemsize(itemsize),
      d_tag_preserve_head_pos(tag_preserve_head_pos)
{
    set_tag_propagation_policy(TPP_DONT);
}

tagged_stream_mux_impl::~tagged_stream_mux_impl() {}

int tagged_stream_mux_impl::calculate_output_stream_length(
    const gr_vector_int& ninput_items)
{
    int nout = 0;
    for (unsigned i = 0; i < ninput_items.size(); i++) {
        nout += ninput_items[i];
    }
    return nout;
}

int tagged_stream_mux_impl::work(int noutput_items,
                                 gr_vector_int& ninput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    unsigned char* out = (unsigned char*)output_items[0];
    int n_produced = 0;

    set_relative_rate((uint64_t)ninput_items.size(), 1);

    for (unsigned int i = 0; i < input_items.size(); i++) {
        const unsigned char* in = (const unsigned char*)input_items[i];

        std::vector<tag_t> tags;
        get_tags_in_range(tags, i, nitems_read(i), nitems_read(i) + ninput_items[i]);
        for (unsigned int j = 0; j < tags.size(); j++) {
            uint64_t offset =
                tags[j].offset - nitems_read(i) + nitems_written(0) + n_produced;
            if (i == d_tag_preserve_head_pos && tags[j].offset == nitems_read(i)) {
                offset -= n_produced;
            }
            d_logger->trace("adding tag {}", tags[j]);
            add_item_tag(0, offset, tags[j].key, tags[j].value);
        }
        memcpy((void*)out, (const void*)in, ninput_items[i] * d_itemsize);
        out += ninput_items[i] * d_itemsize;
        n_produced += ninput_items[i];
    }

    return n_produced;
}

} /* namespace blocks */
} /* namespace gr */
