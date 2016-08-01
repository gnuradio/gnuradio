/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TCP_SERVER_SINK_IMPL_H
#define INCLUDED_GR_TCP_SERVER_SINK_IMPL_H

#include <gnuradio/blocks/tcp_server_sink.h>
#include <boost/asio.hpp>
#include <set>
#include <boost/ptr_container/ptr_vector.hpp>

namespace gr {
  namespace blocks {

    class tcp_server_sink_impl : public tcp_server_sink
    {
    private:
      size_t d_itemsize;

      boost::asio::io_service d_io_service;
      gr::thread::thread d_io_serv_thread;
      boost::asio::ip::tcp::endpoint d_endpoint;
      std::auto_ptr<boost::asio::ip::tcp::socket> d_socket;
      std::set<boost::asio::ip::tcp::socket *> d_sockets;
      boost::asio::ip::tcp::acceptor d_acceptor;

      boost::shared_ptr<uint8_t> d_buf;
      enum {
          BUF_SIZE = 256 * 1024,
      };

      int d_writing;
      boost::condition_variable d_writing_cond;
      boost::mutex d_writing_mut;

      void do_accept(const boost::system::error_code& error);
      void do_write(const boost::system::error_code& error, std::size_t len,
              std::set<boost::asio::ip::tcp::socket *>::iterator);

    public:
      tcp_server_sink_impl(size_t itemsize,
                    const std::string &host, int port,
                    bool noblock);
      ~tcp_server_sink_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TCP_SERVER_SINK_IMPL_H */
