/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stream_demux_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>

namespace gr {
namespace blocks {

stream_demux::sptr stream_demux::make(size_t itemsize, const std::vector<int>& lengths)
{
    return gnuradio::make_block_sptr<stream_demux_impl>(itemsize, lengths);
}

stream_demux_impl::stream_demux_impl(size_t itemsize, const std::vector<int>& lengths)
    : gr::block("stream_demux",
                gr::io_signature::make(1, 1, itemsize),
                gr::io_signature::make(1, -1, itemsize)),
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

void stream_demux_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    std::fill(ninput_items_required.begin(), ninput_items_required.end(), 1);
}

int stream_demux_impl::general_work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    int input_index = 0;                          // Items read
    gr_vector_int output_index(d_lengths.size()); // Items written

    while (input_index < ninput_items[0]) {
        if (noutput_items <= output_index[d_stream]) {
            break;
        }
        const int space_left_in_buffers =
            std::min(ninput_items[0] - input_index,         // Space left in input buffer
                     noutput_items - output_index[d_stream] // Space left in output buffer
            );
        const int items_to_copy = std::min(space_left_in_buffers, d_residual);
        const char* in =
            reinterpret_cast<const char*>(input_items[0]) + input_index * d_itemsize;
        char* out = reinterpret_cast<char*>(output_items[d_stream]);
        memcpy(&out[output_index[d_stream] * d_itemsize], in, items_to_copy * d_itemsize);

        std::vector<gr::tag_t> stream_t;
        get_tags_in_window(stream_t, 0, input_index, input_index + items_to_copy);
        for (auto& t : stream_t) {
            t.offset = t.offset - nitems_read(0) - input_index +
                       nitems_written(d_stream) + output_index[d_stream];
            add_item_tag(d_stream, t);
        }

        output_index[d_stream] += items_to_copy;
        input_index += items_to_copy;
        d_residual -= items_to_copy;
        if (d_residual == 0) {
            do { // Skip all those outputs with zero length
                d_stream = (d_stream + 1) % d_lengths.size();
            } while (d_lengths[d_stream] == 0);
            d_residual = d_lengths[d_stream];
        } else {
            break;
        }
    } // while

    consume(0, input_index);

    for (size_t i = 0; i < output_index.size(); i++) {
        produce((int)i, output_index[i]);
    }

    return WORK_CALLED_PRODUCE;
}

} /* namespace blocks */
} /* namespace gr */
