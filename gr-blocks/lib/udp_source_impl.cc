/* -*- c++ -*- */
/*
 * Copyright 2007-2010,2013 Free Software Foundation, Inc.
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

#include "udp_source_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <stdexcept>
#include <errno.h>
#include <stdio.h>
#include <string.h>

namespace gr {
  namespace blocks {

    udp_source::sptr
    udp_source::make(size_t itemsize,
                     const std::string &ipaddr, int port,
                     int payload_size, bool eof)
    {
      return gnuradio::get_initial_sptr
        (new udp_source_impl(itemsize, ipaddr, port,
                             payload_size, eof));
    }

    udp_source_impl::udp_source_impl(size_t itemsize,
                                     const std::string &host, int port,
                                     int payload_size, bool eof)
      : sync_block("udp_source",
                      io_signature::make(0, 0, 0),
                      io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize), d_payload_size(payload_size),
        d_buf_size(payload_size*50), d_eof(eof),
        d_eof_detected(false), d_connected(false), d_recv_offs(0),
        d_send_offs(0), d_wrap_offs(0)
    {
      // Give us some more room to play.
      d_buf = new char[d_buf_size];
      connect(host, port);
    }

    udp_source_impl::~udp_source_impl()
    {
      if(d_connected)
        disconnect();

      delete [] d_buf;
    }

    void
    udp_source_impl::connect(const std::string &host, int port)
    {
      if(d_connected)
        disconnect();

      d_host = host;
      d_port = static_cast<unsigned short>(port);

      std::string s_port;
      s_port = (boost::format("%d")%d_port).str();

      if(host.size() > 0) {
        boost::asio::ip::udp::resolver resolver(d_io_service);
        boost::asio::ip::udp::resolver::query query(d_host, s_port,
                                                    boost::asio::ip::resolver_query_base::passive);
        d_endpoint = *resolver.resolve(query);

        d_socket = new boost::asio::ip::udp::socket(d_io_service);
        d_socket->open(d_endpoint.protocol());

        boost::asio::socket_base::linger loption(true, 0);
        d_socket->set_option(loption);

        boost::asio::socket_base::reuse_address roption(true);
        d_socket->set_option(roption);

        d_socket->bind(d_endpoint);

        start_receive();
        d_udp_thread = gr::thread::thread(boost::bind(&udp_source_impl::run_io_service, this));
        d_connected = true;
      }
    }

    void
    udp_source_impl::disconnect()
    {
      gr::thread::scoped_lock lock(d_setlock);

      if(!d_connected)
        return;

      d_io_service.reset();
      d_io_service.stop();
      d_udp_thread.join();

      d_socket->close();
      delete d_socket;

      d_connected = false;
    }

    // Return port number of d_socket
    int
    udp_source_impl::get_port(void)
    {
      //return d_endpoint.port();
      return d_socket->local_endpoint().port();
    }

    void
    udp_source_impl::start_receive()
    {
      void *buf = d_buf+d_recv_offs;
      d_socket->async_receive_from(boost::asio::buffer(buf, d_payload_size),
                                   d_endpoint_rcvd,
                                   boost::bind(&udp_source_impl::handle_read, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred));
    }

    void
    udp_source_impl::handle_read(const boost::system::error_code& error,
                                 size_t bytes_transferred)
    {
      boost::lock_guard<gr::thread::mutex> lock(d_udp_mutex);
      if(error) {
        // should not we throw exception in main thread?
        GR_LOG_WARN(d_logger, "ASIO error!");
        d_cond_wait.notify_one();
        return;
      }

      if(bytes_transferred == 1 && d_eof && d_buf[d_recv_offs] == 0x00) {
        d_eof_detected = true;
        d_cond_wait.notify_one();
        return;
      }

      d_recv_offs += bytes_transferred;
      if((d_recv_offs+d_payload_size) > d_buf_size) {
        d_wrap_offs = d_recv_offs;
        d_recv_offs = 0;
      }

      if(!is_buf_full()) {
        start_receive();
      }

      d_cond_wait.notify_one();
    }

    bool
    udp_source_impl::is_buf_full() const
    {
      return d_recv_offs < d_send_offs &&
             (d_recv_offs + d_payload_size) >= d_send_offs;
    }

    int
    udp_source_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock l(d_setlock);
      if(!d_connected)
        return 0;

      char *out = (char*)output_items[0];

      // Use async receive_from to get data from UDP buffer and wait
      // on a conditional signal before proceeding. We use this
      // because the conditional wait is interruptable while a
      // synchronous receive_from is not.
      boost::unique_lock<boost::mutex> lock(d_udp_mutex);

      //use timed_wait to avoid permanent blocking in the work function
      d_cond_wait.timed_wait(lock, boost::posix_time::milliseconds(10));

      if(d_recv_offs == d_send_offs && d_eof_detected) {
        return -1;
      }

      const bool buf_full = is_buf_full();

      int nitems = 0;
      while (d_send_offs != d_recv_offs && noutput_items != nitems) {
        const size_t end_offs = d_recv_offs > d_send_offs ?
              d_recv_offs : d_wrap_offs;
        int dnitems = (end_offs - d_send_offs) / d_itemsize;
        dnitems = std::min(dnitems, noutput_items - nitems);

        memcpy(out, d_buf+d_send_offs, dnitems*d_itemsize);
        nitems += dnitems;

        d_send_offs += dnitems*d_itemsize;
        if(d_send_offs == d_wrap_offs) {
          d_send_offs = 0;
        }
      }

      // start recieveing after (potential) buffer overflow
      if(buf_full && !is_buf_full()) {
        start_receive();
      }

      return nitems;
    }

  } /* namespace blocks */
} /* namespace gr */
