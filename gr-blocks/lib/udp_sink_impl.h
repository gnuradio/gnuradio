/* -*- c++ -*- */
/*
 * Copyright 2007-2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_UDP_SINK_IMPL_H
#define INCLUDED_GR_UDP_SINK_IMPL_H

#include <gnuradio/blocks/udp_sink.h>
#include <boost/asio.hpp>

namespace gr {
namespace blocks {

class udp_sink_impl : public udp_sink
{
private:
    size_t d_itemsize;

    int d_payload_size;        // maximum transmission unit (packet length)
    bool d_eof;                // send zero-length packet on disconnect
    bool d_connected;          // are we connected?
    gr::thread::mutex d_mutex; // protects d_socket and d_connected

    boost::asio::ip::udp::socket* d_socket; // handle to socket
    boost::asio::ip::udp::endpoint d_endpoint;
    boost::asio::io_service d_io_service;

public:
    udp_sink_impl(
        size_t itemsize, const std::string& host, int port, int payload_size, bool eof);
    ~udp_sink_impl();

    int payload_size() { return d_payload_size; }

    void connect(const std::string& host, int port);
    void disconnect();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_UDP_SINK_IMPL_H */
