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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "local_stream_server_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <algorithm>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <gnuradio/thread/thread.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

namespace gr {
  namespace blocks {

    local_stream_server_sink::sptr
      local_stream_server_sink::make(size_t itemsize,
          const std::string &address, bool noblock, congestion_t congestion,
          size_t buf_size)
      {
        return gnuradio::get_initial_sptr
          (new local_stream_server_sink_impl(itemsize, address, noblock,
                                             congestion, buf_size));
      }

    local_stream_server_sink_impl::local_stream_server_sink_impl(
        size_t itemsize, const std::string &address, bool noblock,
        congestion_t congestion, size_t buf_size)
      : sync_block("local_stream_server_sink",
          io_signature::make(1, 1, itemsize),
          io_signature::make(0, 0, 0)),
      d_itemsize(itemsize),
      d_endpoint(address),
      d_acceptor(d_io_service),
      d_congestion(congestion),
      d_buf_size(buf_size),
      d_buf_offs(0),
      d_buf(new uint8_t[buf_size]),
      d_nwriting(0)
      {
        if (address.c_str()[0]) {
          ::unlink(address.c_str());
        }
        d_acceptor.open(d_endpoint.protocol());
        d_acceptor.set_option(boost::asio::local::stream_protocol::acceptor::reuse_address(true));
        d_acceptor.bind(d_endpoint);
        d_acceptor.listen();

        if (!noblock) {
          d_socket.reset(new boost::asio::local::stream_protocol::socket(d_io_service));
          d_acceptor.accept(*d_socket, d_endpoint);
          d_clients.insert(std::make_pair(d_socket.release(), 0));
        }

        d_socket.reset(new boost::asio::local::stream_protocol::socket(d_io_service));
        d_acceptor.async_accept(*d_socket, boost::bind(&local_stream_server_sink_impl::do_accept,
              this, boost::asio::placeholders::error));
        d_io_serv_thread = boost::thread(
            boost::bind(&boost::asio::io_service::run, &d_io_service));
      }

    void
    local_stream_server_sink_impl::do_accept(const boost::system::error_code& error)
    {
      if (!error) {
        gr::thread::scoped_lock guard(d_writing_mut);
        d_clients.insert(std::make_pair(d_socket.release(), 0));
        d_socket.reset(new boost::asio::local::stream_protocol::socket(d_io_service));
        d_acceptor.async_accept(*d_socket, boost::bind(&local_stream_server_sink_impl::do_accept,
              this, boost::asio::placeholders::error));
      }
    }

    void
    local_stream_server_sink_impl::do_write(const boost::system::error_code& error,
        size_t len, client_map_t::iterator i)
    {
      gr::thread::scoped_lock guard(d_writing_mut);

      if (error) {
        delete i->first;
        d_clients.erase(i);
        --d_nwriting;
        d_writing_cond.notify_one();
        return;
      }

      if (i->second < d_buf_offs) {
        const size_t noutput_size = d_buf_offs - i->second;
        void *buf = d_buf.get() + i->second;
        boost::asio::async_write(*i->first, boost::asio::buffer(buf, noutput_size),
            boost::bind(&local_stream_server_sink_impl::do_write, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred,
              i));
        i->second = d_buf_offs;
        d_writing_cond.notify_one();
        return;
      }

      i->second = 0;
      --d_nwriting;
      d_writing_cond.notify_one();
    }

    local_stream_server_sink_impl::~local_stream_server_sink_impl()
    {
      gr::thread::scoped_lock guard(d_writing_mut);
      while (d_nwriting) {
        d_writing_cond.wait(guard);
      }

      BOOST_FOREACH(client_map_t::value_type &i, d_clients) {
        delete i.first;
      }
      d_clients.clear();

      d_io_service.reset();
      d_io_service.stop();
      d_io_serv_thread.join();

      if (d_endpoint.path().c_str()[0]) {
        ::unlink(d_endpoint.path().c_str());
      }
    }

    int
    local_stream_server_sink_impl::work (int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];

      gr::thread::scoped_lock guard(d_writing_mut);

      const size_t noutput_size = noutput_items * d_itemsize;
      if (noutput_size > d_buf_size) {
        throw std::runtime_error("Processing blog bigger than output buffer");
      }
      if (!d_nwriting) {
        d_buf_offs = 0;
      }
      if (noutput_size > (d_buf_size - d_buf_offs)) {
        if (d_congestion == CONGESTION_CLOSE) {
          BOOST_FOREACH(client_map_t::value_type &i, d_clients) {
            if (i.second) {
              i.first->cancel();
              fprintf(stderr, "Socket closed due too congestion\n");
            }
          }
        }
        while (d_nwriting) {
          d_writing_cond.wait(guard);
        }
        d_buf_offs = 0;
      }

      void *buf = d_buf.get() + d_buf_offs;
      memcpy(buf, in, noutput_size);
      d_buf_offs += noutput_size;
      for (client_map_t::iterator i = d_clients.begin();
          i != d_clients.end(); ++i) {
        if (i->second) {
          continue;
        }
        boost::asio::async_write(*i->first, boost::asio::buffer(buf, noutput_size),
            boost::bind(&local_stream_server_sink_impl::do_write, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred,
              i));
        i->second = noutput_size;
      }
      d_nwriting = d_clients.size();

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */

