/* -*- c++ -*- */
/*
 * Copyright 2007-2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "udp_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <cstddef>
#include <cstring>
#include <memory>
#include <stdexcept>

namespace gr {
namespace blocks {

udp_sink::sptr udp_sink::make(
    size_t itemsize, const std::string& host, int port, int payload_size, bool eof)
{
    return gnuradio::make_block_sptr<udp_sink_impl>(
        itemsize, host, port, payload_size, eof);
}

udp_sink_impl::udp_sink_impl(
    size_t itemsize, const std::string& host, int port, int payload_size, bool eof)
    : sync_block(
          "udp_sink", io_signature::make(1, 1, itemsize), io_signature::make(0, 0, 0)),
      d_itemsize(itemsize),
      d_payload_size(payload_size),
      d_eof(eof),
      d_connected(false)
{
    // Get the destination address
    connect(host, port);
}

// public constructor that returns a shared_ptr
udp_sink_impl::~udp_sink_impl()
{
    if (d_connected)
        disconnect();
}

void udp_sink_impl::connect(const std::string& host, int port)
{
    if (d_connected)
        disconnect();

    std::string s_port = (boost::format("%d") % port).str();
    if (!host.empty()) {
        boost::asio::ip::udp::resolver resolver(d_io_service);
        boost::asio::ip::udp::resolver::query query(
            host, s_port, boost::asio::ip::resolver_query_base::passive);
        d_endpoint = *resolver.resolve(query);

        d_socket = std::make_unique<boost::asio::ip::udp::socket>(d_io_service);
        d_socket->open(d_endpoint.protocol());

        boost::asio::socket_base::reuse_address roption(true);
        d_socket->set_option(roption);

        d_connected = true;
    }
}

void udp_sink_impl::disconnect()
{
    if (!d_connected)
        return;

    gr::thread::scoped_lock guard(d_mutex); // protect d_socket from work()

    // Send a few zero-length packets to signal receiver we are done
    boost::array<char, 0> send_buf;
    if (d_eof) {
        int i;
        for (i = 0; i < 3; i++)
            d_socket->send_to(boost::asio::buffer(send_buf), d_endpoint);
    }

    d_socket->close();
    d_socket.reset();

    d_connected = false;
}

int udp_sink_impl::work(int noutput_items,
                        gr_vector_const_void_star& input_items,
                        gr_vector_void_star& output_items)
{
    const char* in = (const char*)input_items[0];
    size_t r = 0;
    std::ptrdiff_t bytes_sent = 0, bytes_to_send = 0;
    const size_t total_size = noutput_items * d_itemsize;

    gr::thread::scoped_lock guard(d_mutex); // protect d_socket

    while (bytes_sent < static_cast<std::ptrdiff_t>(total_size)) {
        bytes_to_send = std::min(static_cast<std::ptrdiff_t>(d_payload_size),
                                 static_cast<std::ptrdiff_t>(total_size - bytes_sent));

        if (d_connected) {
            try {
                r = d_socket->send_to(
                    boost::asio::buffer((void*)(in + bytes_sent), bytes_to_send),
                    d_endpoint);
            } catch (std::exception& e) {
                GR_LOG_ERROR(d_logger, boost::format("send error: %s") % e.what());
                return -1;
            }
        } else
            r = bytes_to_send; // discarded for lack of connection
        bytes_sent += r;
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
