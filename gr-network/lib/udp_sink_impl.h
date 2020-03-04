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

#include <gnuradio/network/udp_sink.h>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/circular_buffer.hpp>

#include <gnuradio/network/packet_headers.h>

namespace gr {
namespace network {

class NETWORK_API udp_sink_impl : public udp_sink
{
protected:
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
    boost::circular_buffer<char>* d_localqueue;
    char* d_localbuffer;

    boost::system::error_code ec;

    boost::asio::io_service d_io_service;
    boost::asio::ip::udp::endpoint d_endpoint;
    boost::asio::ip::udp::socket* d_udpsocket;

    boost::mutex d_mutex;

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
    ~udp_sink_impl();

    bool stop();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace network
} // namespace gr

#endif /* INCLUDED_NETWORK_UDP_SINK_IMPL_H */
