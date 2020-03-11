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

#include <pmt/pmt.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace gr {

class basic_block;

namespace blocks {

class tcp_connection
{
private:
    boost::asio::ip::tcp::socket d_socket;
    std::vector<char> d_buf;
    basic_block* d_block;
    bool d_no_delay;

    tcp_connection(boost::asio::io_service& io_service,
                   int MTU = 10000,
                   bool no_delay = false);

public:
    typedef boost::shared_ptr<tcp_connection> sptr;

    static sptr
    make(boost::asio::io_service& io_service, int MTU = 10000, bool no_delay = false);

    boost::asio::ip::tcp::socket& socket() { return d_socket; };

    void start(gr::basic_block* block);
    void send(pmt::pmt_t vector);
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(boost::shared_ptr<char[]> txbuf,
                      const boost::system::error_code& error,
                      size_t bytes_transferred)
    {
    }
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_TCP_CONNECTION_H */
