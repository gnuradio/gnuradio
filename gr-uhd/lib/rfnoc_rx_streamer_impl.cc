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

#include "gr_uhd_common.h"
#include "rfnoc_rx_streamer_impl.h"
#include <gnuradio/io_signature.h>
#include <uhd/convert.hpp>
#include <uhd/rfnoc/node.hpp>
#include <boost/format.hpp>

const pmt::pmt_t EOB_KEY = pmt::string_to_symbol("rx_eob");
const pmt::pmt_t CMD_TIME_KEY = pmt::mp("time");
const pmt::pmt_t CMD_CHAN_KEY = pmt::mp("chan");
const pmt::pmt_t MSG_PORT_RFNOC = pmt::mp("rfnoc");

namespace gr {
namespace uhd {

/******************************************************************************
 * Factory and Structors
 *****************************************************************************/
rfnoc_rx_streamer::sptr rfnoc_rx_streamer::make(rfnoc_graph::sptr graph,
                                                const size_t num_chans,
                                                const ::uhd::stream_args_t& stream_args,
                                                const size_t vlen,
                                                const bool issue_stream_cmd_on_start)
{
    return gnuradio::make_block_sptr<rfnoc_rx_streamer_impl>(
        graph, num_chans, stream_args, vlen, issue_stream_cmd_on_start);
}


rfnoc_rx_streamer_impl::rfnoc_rx_streamer_impl(rfnoc_graph::sptr graph,
                                               const size_t num_chans,
                                               const ::uhd::stream_args_t& stream_args,
                                               const size_t vlen,
                                               const bool issue_stream_cmd_on_start)
    : gr::sync_block(
          "rfnoc_rx_streamer",
          gr::io_signature::make(0, 0, 0),
          gr::io_signature::make(
              num_chans,
              num_chans,
              ::uhd::convert::get_bytes_per_item(stream_args.cpu_format) * vlen)),
      d_num_chans(num_chans),
      d_itemsize(::uhd::convert::get_bytes_per_item(stream_args.cpu_format)),
      d_vlen(vlen),
      d_graph(graph),
      d_stream_args(stream_args),
      d_streamer(graph->create_rx_streamer(num_chans, stream_args)),
      d_unique_id(
          std::dynamic_pointer_cast<::uhd::rfnoc::node_t>(d_streamer)->get_unique_id()),
      d_issue_stream_cmd_on_start(issue_stream_cmd_on_start)
{
    // nop
}

rfnoc_rx_streamer_impl::~rfnoc_rx_streamer_impl() {}


/******************************************************************************
 * GNU Radio API
 *****************************************************************************/
bool rfnoc_rx_streamer_impl::check_topology(int, int)
{
    GR_LOG_DEBUG(d_logger, "Committing graph...");
    d_graph->commit();
    return true;
}

bool rfnoc_rx_streamer_impl::start()
{
    if (d_issue_stream_cmd_on_start) {
        // Start the streamers
        ::uhd::stream_cmd_t stream_cmd(::uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        if (d_start_time_set) {
            stream_cmd.stream_now = false;
            stream_cmd.time_spec = d_start_time;
            d_start_time_set = false;
        } else {
            stream_cmd.stream_now = true;
        }

        GR_LOG_DEBUG(d_logger, "Sending start stream command...");
        d_streamer->issue_stream_cmd(stream_cmd);
    } else {
        GR_LOG_DEBUG(d_logger, "Starting RX streamer without stream command...");
    }
    return true;
}

bool rfnoc_rx_streamer_impl::stop()
{
    // If we issue a stream command on start, we also issue it on stop
    if (d_issue_stream_cmd_on_start) {
        ::uhd::stream_cmd_t stream_cmd(::uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        d_streamer->issue_stream_cmd(stream_cmd);
    }
    flush();
    return true;
}

int rfnoc_rx_streamer_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const size_t max_num_items_to_rx = noutput_items * d_vlen;
    const size_t num_items_recvd =
        d_streamer->recv(output_items, max_num_items_to_rx, d_metadata, d_timeout);

    const size_t num_vecs_recvd = num_items_recvd / d_vlen;
    if (num_items_recvd % d_vlen) {
        // TODO: Create a fix for this. What will happen is that a partial
        // vector will be received, but it won't be available in the output_items.
        // We need to store the partial vector, and prepend it to the next
        // run.
        GR_LOG_WARN(d_logger, "Received fractional vector! Expect signal fragmentation.");
    }

    using ::uhd::rx_metadata_t;
    switch (d_metadata.error_code) {
    case rx_metadata_t::ERROR_CODE_NONE:
        break;

    case rx_metadata_t::ERROR_CODE_TIMEOUT:
        // its ok to timeout, perhaps the user is doing finite streaming
        GR_LOG_DEBUG(d_logger, "UHD recv() call timed out.");
        break;

    case rx_metadata_t::ERROR_CODE_OVERFLOW:
        // Not much we can do about overruns here, and they get signalled via the
        // UHD logging interface
        break;

    default:
        GR_LOG_WARN(
            d_logger,
            str(boost::format("RFNoC Streamer block received error %s (Code: 0x%x)") %
                d_metadata.strerror() % d_metadata.error_code));
    }

    if (d_metadata.end_of_burst) {
        for (size_t i = 0; i < output_items.size(); i++) {
            add_item_tag(i, nitems_written(i) + num_vecs_recvd - 1, EOB_KEY, pmt::PMT_T);
        }
    }

    return num_vecs_recvd;
}

/******************************************************************************
 * rfnoc_rx_streamer API
 *****************************************************************************/
void rfnoc_rx_streamer_impl::set_start_time(const ::uhd::time_spec_t& time)
{
    d_start_time_set = true;
    d_start_time = time;
}

/******************************************************************************
 * Helpers
 *****************************************************************************/
void rfnoc_rx_streamer_impl::flush()
{
    constexpr size_t nbytes = 4096;
    const size_t nchan = d_streamer->get_num_channels();
    std::vector<std::vector<uint8_t>> buffs(nchan, std::vector<uint8_t>(nbytes));

    gr_vector_void_star outputs;
    for (size_t i = 0; i < nchan; i++) {
        outputs.push_back(&buffs[i].front());
    }

    const size_t itemsize = output_signature()->sizeof_stream_item(0);
    while (true) {
        d_streamer->recv(outputs, nbytes / itemsize / d_vlen, d_metadata, 0.0);
        if (d_metadata.error_code != ::uhd::rx_metadata_t::ERROR_CODE_NONE) {
            break;
        }
    }
}

} /* namespace uhd */
} /* namespace gr */
