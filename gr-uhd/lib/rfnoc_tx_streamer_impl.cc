/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rfnoc_tx_streamer_impl.h"
#include <gnuradio/io_signature.h>
#include <uhd/convert.hpp>
#include <uhd/rfnoc/node.hpp>

namespace gr {
namespace uhd {

/******************************************************************************
 * Factory and Structors
 *****************************************************************************/
rfnoc_tx_streamer::sptr rfnoc_tx_streamer::make(rfnoc_graph::sptr graph,
                                                const size_t num_chans,
                                                const ::uhd::stream_args_t& stream_args,
                                                const size_t vlen)
{
    return gnuradio::make_block_sptr<rfnoc_tx_streamer_impl>(
        graph, num_chans, stream_args, vlen);
}


rfnoc_tx_streamer_impl::rfnoc_tx_streamer_impl(rfnoc_graph::sptr graph,
                                               const size_t num_chans,
                                               const ::uhd::stream_args_t& stream_args,
                                               const size_t vlen)
    : gr::sync_block(
          "rfnoc_tx_streamer",
          gr::io_signature::make(
              num_chans,
              num_chans,
              ::uhd::convert::get_bytes_per_item(stream_args.cpu_format) * vlen),
          gr::io_signature::make(0, 0, 0)),
      d_itemsize(::uhd::convert::get_bytes_per_item(stream_args.cpu_format)),
      d_vlen(vlen),
      d_graph(graph),
      d_stream_args(stream_args),
      d_streamer(graph->create_tx_streamer(num_chans, stream_args)),
      d_unique_id(
          std::dynamic_pointer_cast<::uhd::rfnoc::node_t>(d_streamer)->get_unique_id())
{
    // nop
}

rfnoc_tx_streamer_impl::~rfnoc_tx_streamer_impl() {}

/******************************************************************************
 * GNU Radio API
 *****************************************************************************/
bool rfnoc_tx_streamer_impl::check_topology(int, int)
{
    GR_LOG_DEBUG(d_logger, "Committing graph...");
    d_graph->commit();
    return true;
}

/******************************************************************************
 * GNU Radio API
 *****************************************************************************/
int rfnoc_tx_streamer_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const size_t num_items_to_send = noutput_items * d_vlen;
    const size_t num_items_sent =
        d_streamer->send(input_items, num_items_to_send, d_metadata, d_timeout);
    const size_t num_vecs_sent = num_items_sent / d_vlen;

    if (num_items_sent % d_vlen) {
        // TODO: Create a fix for this. What will happen is that a partial
        // vector will be sent, but it won't be consumed from the input_items.
        // We need to store the offset (what fraction of the vector was sent)
        // and tx that first.
        GR_LOG_WARN(d_logger, "Sent fractional vector! Expect signal fragmentation.");
    }

    return num_vecs_sent;
}

} /* namespace uhd */
} /* namespace gr */
