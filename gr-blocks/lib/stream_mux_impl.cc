/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stream_mux_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>

namespace gr {
namespace blocks {

stream_mux::sptr stream_mux::make(size_t itemsize, const std::vector<int>& lengths)
{
    return gnuradio::make_block_sptr<stream_mux_impl>(itemsize, lengths);
}

stream_mux_impl::stream_mux_impl(size_t itemsize, const std::vector<int>& lengths)
    : block("stream_mux",
            io_signature::make(1, -1, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_itemsize(itemsize),
      d_lengths(lengths)
{
    while (d_lengths[d_stream] == 0) {
        d_stream++;
        if (d_stream == d_lengths.size()) {
            throw std::invalid_argument("At least one size must be non-zero.");
        }
    }
    d_residual = d_lengths[d_stream];
    set_tag_propagation_policy(TPP_DONT);
}

void stream_mux_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++) {
        // Only active inputs *need* items, for the rest, it would just be nice
        ninput_items_required[i] = (d_stream == i ? 1 : 0);
    }
}

int stream_mux_impl::general_work(int noutput_items,
                                  gr_vector_int& ninput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{

    int out_index = 0;                           // Items written
    gr_vector_int input_index(d_lengths.size()); // Items read

    while (out_index < noutput_items) {
        if (ninput_items[d_stream] <= input_index[d_stream]) {
            break;
        }
        const int space_left_in_buffers = std::min(
            noutput_items - out_index,                     // Space left in output buffer
            ninput_items[d_stream] - input_index[d_stream] // Space left in input buffer
        );
        const int items_to_copy = std::min(space_left_in_buffers, d_residual);
        const char* in = reinterpret_cast<const char*>(input_items[d_stream]) +
                         input_index[d_stream] * d_itemsize;
        char* out = reinterpret_cast<char*>(output_items[0]);
        memcpy(&out[out_index * d_itemsize], in, items_to_copy * d_itemsize);

        std::vector<gr::tag_t> stream_t;
        get_tags_in_window(stream_t,
                           d_stream,
                           input_index[d_stream],
                           input_index[d_stream] + items_to_copy);
        for (auto& t : stream_t) {
            t.offset = t.offset - nitems_read(d_stream) - input_index[d_stream] +
                       nitems_written(0) + out_index;
            add_item_tag(0, t);
        }

        out_index += items_to_copy;
        input_index[d_stream] += items_to_copy;
        d_residual -= items_to_copy;
        if (d_residual == 0) {
            do { // Skip all those inputs with zero length
                d_stream = (d_stream + 1) % d_lengths.size();
            } while (d_lengths[d_stream] == 0);
            d_residual = d_lengths[d_stream];
        } else {
            break;
        }
    } // while

    for (size_t i = 0; i < input_index.size(); i++) {
        consume((int)i, input_index[i]);
    }

    return out_index;
} /* work */

} /* namespace blocks */
} /* namespace gr */
