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

#include "tcp_server_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include <stdio.h>
#include <string.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/make_unique.hpp>
#include <algorithm>
#include <stdexcept>

namespace gr {
namespace blocks {

tcp_server_sink::sptr
tcp_server_sink::make(size_t itemsize, const std::string& host, int port, bool noblock)
{
    return gnuradio::make_block_sptr<tcp_server_sink_impl>(itemsize, host, port, noblock);
}

tcp_server_sink_impl::tcp_server_sink_impl(size_t itemsize,
                                           const std::string& host,
                                           int port,
                                           bool noblock)
    : sync_block("tcp_server_sink",
                 io_signature::make(1, 1, itemsize),
                 io_signature::make(0, 0, 0)),
      d_itemsize(itemsize),
      d_acceptor(d_io_service),
      d_writing(0)
{
    std::string s__port = (boost::format("%d") % port).str();
    std::string s__host = host.empty() ? std::string("localhost") : host;
    boost::asio::ip::tcp::resolver resolver(d_io_service);
    boost::asio::ip::tcp::resolver::query query(
        s__host, s__port, boost::asio::ip::resolver_query_base::passive);
    d_endpoint = *resolver.resolve(query);

    d_acceptor.open(d_endpoint.protocol());
    d_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    d_acceptor.bind(d_endpoint);
    d_acceptor.listen();

    if (!noblock) {
        auto sock = boost::make_unique<boost::asio::ip::tcp::socket>(d_io_service);
        d_acceptor.accept(*sock, d_endpoint);
        d_sockets.insert(std::move(sock));
    }

    d_socket = boost::make_unique<boost::asio::ip::tcp::socket>(d_io_service);
    d_acceptor.async_accept(*d_socket,
                            boost::bind(&tcp_server_sink_impl::do_accept,
                                        this,
                                        boost::asio::placeholders::error));
    d_io_serv_thread =
        boost::thread(boost::bind(&boost::asio::io_service::run, &d_io_service));
}

void tcp_server_sink_impl::do_accept(const boost::system::error_code& error)
{
    if (!error) {
        gr::thread::scoped_lock guard(d_writing_mut);
        d_sockets.insert(std::move(d_socket));
        d_socket = boost::make_unique<boost::asio::ip::tcp::socket>(d_io_service);
        d_acceptor.async_accept(*d_socket,
                                boost::bind(&tcp_server_sink_impl::do_accept,
                                            this,
                                            boost::asio::placeholders::error));
    }
}

void tcp_server_sink_impl::do_write(
    const boost::system::error_code& error,
    size_t len,
    std::set<std::unique_ptr<boost::asio::ip::tcp::socket>>::iterator i)
{
    {
        gr::thread::scoped_lock guard(d_writing_mut);
        --d_writing;
        if (error) {
            d_sockets.erase(i);
        }
    }
    d_writing_cond.notify_one();
}

tcp_server_sink_impl::~tcp_server_sink_impl()
{
    gr::thread::scoped_lock guard(d_writing_mut);
    while (d_writing) {
        d_writing_cond.wait(guard);
    }
    d_io_service.reset();
    d_io_service.stop();
    d_io_serv_thread.join();
}

int tcp_server_sink_impl::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    const char* in = (const char*)input_items[0];

    gr::thread::scoped_lock guard(d_writing_mut);
    while (d_writing) {
        d_writing_cond.wait(guard);
    }

    size_t data_len = std::min(size_t(BUF_SIZE), noutput_items * d_itemsize);
    data_len -= data_len % d_itemsize;
    memcpy(d_buf.data(), in, data_len);
    for (auto i = std::begin(d_sockets); i != std::end(d_sockets); ++i) {
        boost::asio::async_write(**i,
                                 boost::asio::buffer(d_buf.data(), data_len),
                                 boost::bind(&tcp_server_sink_impl::do_write,
                                             this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred,
                                             i));
    }
    d_writing = d_sockets.size();

    return data_len / d_itemsize;
}

} /* namespace blocks */
} /* namespace gr */
