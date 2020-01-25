/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TCP_SERVER_SINK_IMPL_H
#define INCLUDED_GR_TCP_SERVER_SINK_IMPL_H

#include <gnuradio/blocks/tcp_server_sink.h>
#include <boost/asio.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <set>

namespace gr {
namespace blocks {

class tcp_server_sink_impl : public tcp_server_sink
{
private:
    size_t d_itemsize;

    boost::asio::io_service d_io_service;
    gr::thread::thread d_io_serv_thread;
    boost::asio::ip::tcp::endpoint d_endpoint;
    std::unique_ptr<boost::asio::ip::tcp::socket> d_socket;
    std::set<boost::asio::ip::tcp::socket*> d_sockets;
    boost::asio::ip::tcp::acceptor d_acceptor;

    boost::shared_ptr<uint8_t[]> d_buf;
    enum {
        BUF_SIZE = 256 * 1024,
    };

    int d_writing;
    boost::condition_variable d_writing_cond;
    boost::mutex d_writing_mut;

    void do_accept(const boost::system::error_code& error);
    void do_write(const boost::system::error_code& error,
                  std::size_t len,
                  std::set<boost::asio::ip::tcp::socket*>::iterator);

public:
    tcp_server_sink_impl(size_t itemsize,
                         const std::string& host,
                         int port,
                         bool noblock);
    ~tcp_server_sink_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TCP_SERVER_SINK_IMPL_H */
