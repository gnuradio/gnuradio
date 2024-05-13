/* -*- c++ -*- */
/*
 * Copyright (C) 2023-2024 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "burst_to_stream_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdint>
#include <cstring>
#include <vector>

namespace gr {
namespace blocks {

burst_to_stream::sptr burst_to_stream::make(size_t itemsize,
                                            const std::string& len_tag_key,
                                            bool propagate_tags)
{
    return gnuradio::make_block_sptr<burst_to_stream_impl>(
        itemsize, len_tag_key, propagate_tags);
}

burst_to_stream_impl::burst_to_stream_impl(size_t itemsize,
                                           const std::string& len_tag_key,
                                           bool propagate_tags)
    : gr::block("burst_to_stream",
                gr::io_signature::make(1, 1, itemsize),
                gr::io_signature::make(1, 1, itemsize)),
      d_itemsize(itemsize),
      d_len_tag_key(pmt::string_to_symbol(len_tag_key)),
      d_propagate_tags(propagate_tags),
      d_remaining_items(0)
{
    set_tag_propagation_policy(TPP_DONT);
}

burst_to_stream_impl::~burst_to_stream_impl() {}

void burst_to_stream_impl::forecast(int noutput_items,
                                    gr_vector_int& ninput_items_required)
{
    if (d_remaining_items > 0) {
        ninput_items_required[0] =
            std::min(d_remaining_items, static_cast<size_t>(noutput_items));
    } else {
        ninput_items_required[0] = 0;
    }
}

int burst_to_stream_impl::general_work(int noutput_items,
                                       gr_vector_int& ninput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    auto in = static_cast<const uint8_t*>(input_items[0]);
    auto out = static_cast<uint8_t*>(output_items[0]);
    const auto out_end = out + d_itemsize * noutput_items;
    int consumed = 0;
    int produced = 0;

    while (out < out_end) {
        if (d_remaining_items == 0) {
            // Try to fetch a new packet from the input
            if (consumed == ninput_items[0]) {
                // There is no more input available. Set the remaining part of the output
                // to zero.
                std::memset(out, 0, out_end - out);
                produced = noutput_items;
                break;
            }

            const auto nitems = nitems_read(0) + consumed;
            std::vector<gr::tag_t> tags;
            get_tags_in_range(tags, 0, nitems, nitems + 1, d_len_tag_key);
            if (tags.empty()) {
                throw std::runtime_error(
                    "burst_to_stream_impl: expected packet length tag not found");
            }
            d_remaining_items = pmt::to_long(tags[0].value);
        }
        const size_t input_available = ninput_items[0] - consumed;
        if (input_available == 0) {
            // We are mid-packet but have no more input. Return to wait for the
            // rest of the packet.
            break;
        }
        const auto produce_items =
            std::min({ d_remaining_items,
                       input_available,
                       static_cast<size_t>(noutput_items - produced) });
        const auto produce_bytes = d_itemsize * produce_items;
        std::memcpy(out, in, produce_bytes);

        if (d_propagate_tags) {
            std::vector<gr::tag_t> tags;
            const auto nitems = nitems_read(0) + consumed;
            get_tags_in_range(tags, 0, nitems, nitems + produce_items);
            for (const auto& tag : tags) {
                const auto offset = tag.offset - nitems + nitems_written(0) + produced;
                add_item_tag(0, offset, tag.key, tag.value);
            }
        }

        out += produce_bytes;
        in += produce_bytes;
        consumed += produce_items;
        produced += produce_items;
        d_remaining_items -= produce_items;
    }

    consume(0, consumed);

    return produced;
}

} /* namespace blocks */
} /* namespace gr */
