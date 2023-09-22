/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TCP_CONNECTION_H
#define INCLUDED_TCP_CONNECTION_H

#include <asio.hpp>
#include <pmt/pmt.h>
#include <memory>

namespace gr {

class basic_block;

namespace network {

class tcp_connection
{
private:
    asio::ip::tcp::socket d_socket;
    std::vector<char> d_buf;
    basic_block* d_block;
    bool d_no_delay;

    tcp_connection(asio::io_context& io_context, int MTU = 10000, bool no_delay = false);

    void handle_read(const asio::error_code& error, size_t bytes_transferred);

public:
    typedef std::shared_ptr<tcp_connection> sptr;

    static sptr
    make(asio::io_context& io_context, int MTU = 10000, bool no_delay = false);

    asio::ip::tcp::socket& socket() { return d_socket; };

    void start(gr::basic_block* block);
    void send(pmt::pmt_t vector);
};

} /* namespace network */
} /* namespace gr */

#endif /* INCLUDED_TCP_CONNECTION_H */
