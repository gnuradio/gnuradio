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

#include "tagged_stream_align_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

tagged_stream_align::sptr tagged_stream_align::make(size_t itemsize,
                                                    const std::string& lengthtagname)
{
    return gnuradio::make_block_sptr<tagged_stream_align_impl>(itemsize, lengthtagname);
}

tagged_stream_align_impl::tagged_stream_align_impl(size_t itemsize,
                                                   const std::string& lengthtagname)
    : block("tagged_stream_align",
            io_signature::make(1, 1, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_itemsize(itemsize),
      d_lengthtag(pmt::mp(lengthtagname)),
      d_have_sync(false)
{
    set_tag_propagation_policy(TPP_DONT);
}

tagged_stream_align_impl::~tagged_stream_align_impl() {}

int tagged_stream_align_impl::general_work(int noutput_items,
                                           gr_vector_int& ninput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    std::vector<tag_t> tags;
    if (d_have_sync) {
        int ncp = std::min(noutput_items, ninput_items[0]);
        get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items);
        for (size_t i = 0; i < tags.size(); i++) {
            gr::tag_t t = tags[i];
            int offset = (nitems_read(0) - nitems_written(0));
            t.offset -= offset;
            add_item_tag(0, t);
        }
        memcpy(output_items[0], input_items[0], ncp * d_itemsize);
        consume_each(ncp);
        return ncp;
    } else {
        get_tags_in_range(
            tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0], d_lengthtag);
        if (!tags.empty()) {
            d_have_sync = true;
            consume_each(tags[0].offset - nitems_read(0));
        } else {
            consume_each(ninput_items[0]);
        }
        return 0;
    }
}

} /* namespace blocks */
} /* namespace gr */
