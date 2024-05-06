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

#include "tag_gate_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

tag_gate::sptr tag_gate::make(size_t item_size, bool propagate_tags)
{
    return gnuradio::make_block_sptr<tag_gate_impl>(item_size, propagate_tags);
}

tag_gate_impl::tag_gate_impl(size_t item_size, bool propagate_tags)
    : gr::sync_block("tag_gate",
                     gr::io_signature::make(1, 1, item_size),
                     gr::io_signature::make(1, 1, item_size)),
      d_item_size(item_size),
      d_propagate_tags(propagate_tags),
      d_single_key_set(false)
{
    if (!d_propagate_tags) {
        set_tag_propagation_policy(TPP_DONT);
    }
    d_single_key = pmt::PMT_NIL;
}

tag_gate_impl::~tag_gate_impl() {}

void tag_gate_impl::set_propagation(bool propagate_tags)
{
    d_propagate_tags = propagate_tags;

    if (propagate_tags) {
        set_tag_propagation_policy(TPP_ALL_TO_ALL);
    } else {
        set_tag_propagation_policy(TPP_DONT);
    }
}

void tag_gate_impl::set_single_key(const std::string& single_key)
{
    if (single_key.empty()) {
        d_single_key = pmt::PMT_NIL;
        d_single_key_set = false;
    } else {
        d_single_key = pmt::intern(single_key);
        d_single_key_set = true;
    }
}

std::string tag_gate_impl::single_key() const
{
    if (pmt::equal(d_single_key, pmt::PMT_NIL)) {
        return "";
    } else {
        return pmt::symbol_to_string(d_single_key);
    }
}

int tag_gate_impl::work(int noutput_items,
                        gr_vector_const_void_star& input_items,
                        gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];
    std::vector<tag_t> tags;

    memcpy((void*)out, (const void*)in, d_item_size * noutput_items);

    if (d_single_key_set && (!d_propagate_tags)) {
        get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items);
        for (unsigned int i = 0; i < tags.size(); i++) {
            if (!pmt::equal(tags[i].key, d_single_key))
                add_item_tag(
                    0, tags[i].offset, tags[i].key, tags[i].value, tags[i].srcid);
        }
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
