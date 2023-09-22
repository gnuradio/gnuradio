/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_UDP_SINK_IMPL_H
#define INCLUDED_NETWORK_UDP_SINK_IMPL_H

#include <gnuradio/buffer.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/network/udp_sink.h>
#include <asio.hpp>

#include <gnuradio/network/packet_headers.h>

namespace gr {
namespace network {

class NETWORK_API udp_sink_impl : public udp_sink
{
protected:
    std::string d_host;
    int d_port;
    size_t d_itemsize;
    size_t d_veclen;
    size_t d_block_size;

    bool is_ipv6;
    int d_header_type;
    int d_header_size;
    uint64_t d_seq_num;
    uint16_t d_payloadsize;
    bool b_send_eof;

    int d_precomp_datasize;
    int d_precomp_data_overitemsize;

    char d_tmpheaderbuff[12]; // Largest header is 10 bytes

    // A queue is required because we have 2 different timing
    // domains: The network packets and the GR work()/scheduler
    gr::buffer_sptr d_localqueue_writer;
    gr::buffer_reader_sptr d_localqueue_reader;

    asio::error_code ec;

    asio::io_context d_io_context;
    asio::ip::udp::endpoint d_endpoint;
    asio::ip::udp::socket* d_udpsocket = nullptr;

    virtual void
    build_header(); // returns header size.  Header is stored in tmpHeaderBuff

public:
    udp_sink_impl(size_t itemsize,
                  size_t veclen,
                  const std::string& host,
                  int port,
                  int header_type = HEADERTYPE_NONE,
                  int payloadsize = 1472,
                  bool send_eof = true);
    ~udp_sink_impl() override;

    bool start() override;
    bool stop() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace network
} // namespace gr

#endif /* INCLUDED_NETWORK_UDP_SINK_IMPL_H */
