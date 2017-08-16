/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_TCP_CONNECTION_H
#define INCLUDED_TCP_CONNECTION_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <pmt/pmt.h>

namespace gr {

  class basic_block;

  namespace blocks {

    class tcp_connection
    {
    private:
      boost::asio::ip::tcp::socket d_socket;
      std::vector<char> d_buf;
      basic_block *d_block;
      bool d_no_delay;

      tcp_connection(boost::asio::io_service& io_service, int MTU=10000, bool no_delay=false);

    public:
      typedef boost::shared_ptr<tcp_connection> sptr;

      static sptr make(boost::asio::io_service& io_service, int MTU=10000, bool no_delay=false);

      boost::asio::ip::tcp::socket& socket() { return d_socket; };

      void start(gr::basic_block *block);
      void send(pmt::pmt_t vector);
      void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
      void handle_write(boost::shared_ptr<char[]> txbuf, const boost::system::error_code& error,
                        size_t bytes_transferred) { }
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_TCP_CONNECTION_H */
