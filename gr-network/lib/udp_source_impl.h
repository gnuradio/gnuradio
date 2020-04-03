/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_UDP_SOURCE_IMPL_H
#define INCLUDED_NETWORK_UDP_SOURCE_IMPL_H

#include <gnuradio/network/udp_source.h>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/circular_buffer.hpp>

#include <gnuradio/network/packet_headers.h>

namespace gr {
namespace network {

class NETWORK_API udp_source_impl : public udp_source
{
protected:
    bool is_ipv6;
    size_t d_itemsize;
    size_t d_veclen;
    int d_port;

    bool d_notify_missed;
    bool d_source_zeros;
    int d_header_type;
    uint16_t d_payloadsize;

    uint64_t d_seq_num;
    int d_header_size;
    int d_partial_frame_counter;

    int d_precomp_data_size;
    int d_precomp_data_over_item_size;
    size_t d_block_size;

    char* d_local_buffer;

    boost::system::error_code ec;

    boost::asio::io_service d_io_service;
    boost::asio::ip::udp::endpoint d_endpoint;
    boost::asio::ip::udp::socket* d_udpsocket;

    boost::asio::streambuf d_read_buffer;

    // A queue is required because we have 2 different timing
    // domains: The network packets and the GR work()/scheduler
    boost::circular_buffer<char>* d_localqueue;

    uint64_t get_header_seqnum();

public:
    udp_source_impl(size_t itemsize,
                    size_t veclen,
                    int port,
                    int header_type,
                    int payloadsize,
                    bool notify_missed,
                    bool source_zeros,
                    bool ipv6);
    ~udp_source_impl();

    bool stop();

    size_t data_available();
    inline size_t netdata_available();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace network
} // namespace gr

#endif /* INCLUDED_NETWORK_UDP_SOURCE_IMPL_H */
