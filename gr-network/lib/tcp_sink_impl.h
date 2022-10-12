/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_TCP_SINK_IMPL_H
#define INCLUDED_NETWORK_TCP_SINK_IMPL_H

#include <gnuradio/network/tcp_sink.h>
#include <asio.hpp>
#include <thread>

namespace gr {
namespace network {

class NETWORK_API tcp_sink_impl : public tcp_sink
{
protected:
    size_t d_itemsize;
    size_t d_veclen;
    std::string d_host;
    int d_port;
    int d_sinkmode;

    bool d_thread_running;
    bool d_stop_thread;
    std::thread* d_listener_thread;
    bool d_start_new_listener;
    bool d_initial_connection;

    size_t d_block_size;
    bool d_is_ipv6;

    asio::error_code ec;

    asio::io_context d_io_context;
    asio::ip::tcp::endpoint d_endpoint;
    asio::ip::tcp::socket* d_tcpsocket = NULL;
    asio::ip::tcp::acceptor* d_acceptor = NULL;

    bool d_connected;

    virtual void connect(bool initial_connection);
    virtual void run_listener();

public:
    tcp_sink_impl(size_t itemsize,
                  size_t veclen,
                  const std::string& host,
                  int port,
                  int sinkmode = TCPSINKMODE_CLIENT);
    ~tcp_sink_impl() override;

    bool start() override;
    bool stop() override;

    void accept_handler(asio::ip::tcp::socket* new_connection,
                        const asio::error_code& error);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace network
} // namespace gr

#endif /* INCLUDED_NETWORK_TCP_SINK_IMPL_H */
