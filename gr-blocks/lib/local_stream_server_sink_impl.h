/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_LOCAL_STREAM_SERVER_SINK_IMPL_H
#define INCLUDED_GR_LOCAL_STREAM_SERVER_SINK_IMPL_H

#include <gnuradio/blocks/local_stream_server_sink.h>
#include <boost/asio.hpp>
#include <map>
#include <boost/ptr_container/ptr_vector.hpp>

namespace gr {
  namespace blocks {

    class local_stream_server_sink_impl : public local_stream_server_sink
    {
    private:
      // store socket and number of bytes writen by currend send_async operation
      typedef std::map<boost::asio::local::stream_protocol::socket *, size_t> client_map_t;

      size_t d_itemsize;

      boost::asio::io_service d_io_service;
      gr::thread::thread d_io_serv_thread;
      boost::asio::local::stream_protocol::endpoint d_endpoint;
      std::auto_ptr<boost::asio::local::stream_protocol::socket> d_socket;
      client_map_t d_clients;
      boost::asio::local::stream_protocol::acceptor d_acceptor;

      congestion_t d_congestion;
      size_t d_buf_size;
      size_t d_buf_offs;
      boost::shared_ptr<uint8_t> d_buf;

      int d_nwriting;
      boost::condition_variable d_writing_cond;
      boost::mutex d_writing_mut;

      void do_accept(const boost::system::error_code& error);
      void do_write(const boost::system::error_code& error, std::size_t len,
              client_map_t::iterator);

    public:
      local_stream_server_sink_impl(size_t itemsize,
                    const std::string &address, bool noblock,
                    congestion_t congestion, size_t buf_size);
      ~local_stream_server_sink_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_LOCAL_STREAM_SERVER_SINK_IMPL_H */
