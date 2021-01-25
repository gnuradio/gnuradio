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

#include "tags_strobe_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>

namespace gr {
namespace blocks {

tags_strobe::sptr tags_strobe::make(size_t sizeof_stream_item,
                                    pmt::pmt_t value,
                                    uint64_t nsamps,
                                    pmt::pmt_t key)
{
    return gnuradio::make_block_sptr<tags_strobe_impl>(
        sizeof_stream_item, value, nsamps, key);
}

tags_strobe_impl::tags_strobe_impl(size_t sizeof_stream_item,
                                   pmt::pmt_t value,
                                   uint64_t nsamps,
                                   pmt::pmt_t key)
    : sync_block("tags_strobe",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof_stream_item)),
      d_itemsize(sizeof_stream_item)
{
    d_tag.offset = 0;
    d_tag.key = pmt::intern("strobe");
    d_tag.srcid = alias_pmt();
    set_value(value);
    set_key(key);
    set_nsamps(nsamps);
    d_offset = 0;
}

tags_strobe_impl::~tags_strobe_impl() {}

void tags_strobe_impl::set_value(pmt::pmt_t value) { d_tag.value = value; }

void tags_strobe_impl::set_key(pmt::pmt_t key) { d_tag.key = key; }

void tags_strobe_impl::set_nsamps(uint64_t nsamps) { d_nsamps = nsamps; }

int tags_strobe_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    void* optr = (void*)output_items[0];
    memset(optr, 0, noutput_items * d_itemsize);

    uint64_t nitems = static_cast<uint64_t>(noutput_items) + nitems_written(0);
    while ((nitems - d_offset) > d_nsamps) {
        d_offset += d_nsamps;
        d_tag.offset = d_offset;
        add_item_tag(0, d_tag);
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
