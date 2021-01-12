/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tagged_stream_multiply_length_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

tagged_stream_multiply_length::sptr tagged_stream_multiply_length::make(
    size_t itemsize, const std::string& lengthtagname, double scalar)
{
    return gnuradio::make_block_sptr<tagged_stream_multiply_length_impl>(
        itemsize, lengthtagname, scalar);
}

tagged_stream_multiply_length_impl::tagged_stream_multiply_length_impl(
    size_t itemsize, const std::string& lengthtagname, double scalar)
    : block("tagged_stream_multiply_length",
            io_signature::make(1, 1, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_lengthtag(pmt::mp(lengthtagname)),
      d_scalar(scalar),
      d_itemsize(itemsize)
{
    set_tag_propagation_policy(TPP_DONT);
    set_relative_rate(1, 1);
    message_port_register_in(pmt::intern("set_scalar"));
    set_msg_handler(pmt::intern("set_scalar"),
                    [this](pmt::pmt_t msg) { this->set_scalar_pmt(msg); });
}

tagged_stream_multiply_length_impl::~tagged_stream_multiply_length_impl() {}

int tagged_stream_multiply_length_impl::general_work(
    int noutput_items,
    gr_vector_int& ninput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items)
{
    const void* in = input_items[0];
    void* out = output_items[0];

    // move data across ( wasteful memcopy :< )
    memcpy(out, in, noutput_items * d_itemsize);

    // move and update tags
    std::vector<tag_t> tags;
    get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items);
    for (size_t i = 0; i < tags.size(); i++) {
        if (pmt::eqv(tags[i].key, d_lengthtag)) {
            // propagate with value update (scaled)
            add_item_tag(0,
                         tags[i].offset,
                         tags[i].key,
                         pmt::from_long(pmt::to_long(tags[i].value) * d_scalar),
                         tags[i].srcid);
        } else {
            // propagate unmodified
            add_item_tag(0, tags[i].offset, tags[i].key, tags[i].value, tags[i].srcid);
        }
    }

    consume_each(noutput_items);
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
