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
#include <gnuradio/uhd/cmd_keys.h>
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
      d_streamer(graph->create_tx_streamer(std::max<size_t>(1U, num_chans), stream_args)),
      d_unique_id(
          std::dynamic_pointer_cast<::uhd::rfnoc::node_t>(d_streamer)->get_unique_id())
{
    // Register message port
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { handle_pdu(msg); });
}

rfnoc_tx_streamer_impl::~rfnoc_tx_streamer_impl() {}

/******************************************************************************
 * Message Handlers
 *****************************************************************************/
void rfnoc_tx_streamer_impl::handle_pdu(const pmt::pmt_t& msg)
{
    pmt::pmt_t metadata;
    pmt::pmt_t data;

    // Check if message is a pair (PDU format) or a direct vector
    if (pmt::is_pair(msg)) {
        // Extract metadata (car) and data (cdr)
        metadata = pmt::car(msg);
        data = pmt::cdr(msg);

        // Log received PDU
        d_logger->debug("Received PDU with metadata: {}", pmt::write_string(metadata));
    } else if (pmt::is_vector(msg)) {
        // Message is directly a vector, treat as data with empty metadata
        metadata = pmt::make_dict();
        data = msg;

        d_logger->debug("Received direct vector message (no metadata)");
    } else {
        d_logger->warn("Received message is neither a pair (PDU format) nor a vector");
        return;
    }

    // Get data vector properties
    size_t data_len = 0;

    // Create input items vector for streamer
    std::vector<const void*> input_items;

    // Get the CPU format for validation
    const std::string& cpu_format = d_stream_args.cpu_format;

    // Lambda to handle PDU type checking and validation
    auto handle_pdu_type = [&](bool (*type_check_func)(pmt::pmt_t),
                               const void* (*pmt_get_elements_func)(pmt::pmt_t, size_t&),
                               const std::string& expected_format,
                               const std::string& type_name) -> bool {
        if (type_check_func(data)) {
            if (cpu_format != expected_format) {
                d_logger->warn(
                    "PDU contains {} data but streamer expects cpu_format '{}' not '{}'",
                    type_name,
                    cpu_format,
                    expected_format);
                return false;
            }
            input_items.push_back(
                static_cast<const void*>(pmt_get_elements_func(data, data_len)));
            return true;
        }
        return false;
    };

    // Validate that PMT type matches the expected CPU format
    const bool type_handled =
        handle_pdu_type(
            pmt::is_c32vector,
            [](pmt::pmt_t data, size_t& len) {
                return static_cast<const void*>(pmt::c32vector_elements(data, len));
            },
            "fc32",
            "c32vector") ||
        handle_pdu_type(
            pmt::is_c64vector,
            [](pmt::pmt_t data, size_t& len) {
                return static_cast<const void*>(pmt::c64vector_elements(data, len));
            },
            "fc64",
            "c64vector") ||
        handle_pdu_type(
            pmt::is_f32vector,
            [](pmt::pmt_t data, size_t& len) {
                return static_cast<const void*>(pmt::f32vector_elements(data, len));
            },
            "f32",
            "f32vector") ||
        handle_pdu_type(
            pmt::is_f64vector,
            [](pmt::pmt_t data, size_t& len) {
                return static_cast<const void*>(pmt::f64vector_elements(data, len));
            },
            "f64",
            "f64vector") ||
        handle_pdu_type(
            pmt::is_s16vector,
            [](pmt::pmt_t data, size_t& len) {
                return static_cast<const void*>(pmt::s16vector_elements(data, len));
            },
            "s16",
            "s16vector") ||
        handle_pdu_type(
            pmt::is_s32vector,
            [](pmt::pmt_t data, size_t& len) {
                return static_cast<const void*>(pmt::s32vector_elements(data, len));
            },
            "s32",
            "s32vector");

    if (!type_handled) {
        d_logger->warn("Unsupported data vector type in PDU (expected format: '{}')",
                       cpu_format);
        return;
    }
    if (data_len == 0) {
        d_logger->warn("PDU data vector is empty");
        return;
    }

    // Parse metadata for TX metadata fields
    ::uhd::tx_metadata_t tx_metadata;
    tx_metadata.start_of_burst = true;
    tx_metadata.end_of_burst = true;
    tx_metadata.has_time_spec = false;

    if (pmt::is_dict(metadata)) {
        // Check for start of burst
        if (pmt::dict_has_key(metadata, cmd_tx_sob_key())) {
            tx_metadata.start_of_burst =
                pmt::to_bool(pmt::dict_ref(metadata, cmd_tx_sob_key(), pmt::PMT_T));
        }

        // Check for end of burst
        if (pmt::dict_has_key(metadata, cmd_tx_eob_key())) {
            tx_metadata.end_of_burst =
                pmt::to_bool(pmt::dict_ref(metadata, cmd_tx_eob_key(), pmt::PMT_T));
        }

        // Check for time specification
        if (pmt::dict_has_key(metadata, cmd_time_key())) {
            pmt::pmt_t tx_time = pmt::dict_ref(metadata, cmd_time_key(), pmt::PMT_NIL);
            if (pmt::is_tuple(tx_time) && pmt::length(tx_time) >= 2) {
                uint64_t full_secs = pmt::to_uint64(pmt::tuple_ref(tx_time, 0));
                double frac_secs = pmt::to_double(pmt::tuple_ref(tx_time, 1));
                tx_metadata.time_spec = ::uhd::time_spec_t(full_secs, frac_secs);
                tx_metadata.has_time_spec = true;
            }
        }
    }

    // Calculate number of items considering vector length
    const size_t num_items_to_send = data_len / d_vlen;
    if (data_len % d_vlen != 0) {
        d_logger->warn(
            "PDU data length {} is not a multiple of vector length {}", data_len, d_vlen);
    }

    if (num_items_to_send > 0) {
        // Send data via streamer
        try {
            const size_t num_items_sent =
                d_streamer->send(input_items, num_items_to_send, tx_metadata, d_timeout);

            d_logger->debug("Sent {} items via streamer (requested: {})",
                            num_items_sent,
                            num_items_to_send);

            if (num_items_sent != num_items_to_send) {
                d_logger->warn("Only sent {} of {} requested items",
                               num_items_sent,
                               num_items_to_send);
            }
        } catch (const std::exception& e) {
            d_logger->error("Error sending PDU data: {}", e.what());
        }
    } else {
        d_logger->warn("No complete vectors to send from PDU data");
    }
}

/******************************************************************************
 * GNU Radio API
 *****************************************************************************/
bool rfnoc_tx_streamer_impl::check_topology(int, int)
{
    d_logger->debug("Committing graph...");
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
        d_logger->warn("Sent fractional vector! Expect signal fragmentation.");
    }

    return num_vecs_sent;
}

} /* namespace uhd */
} /* namespace gr */
