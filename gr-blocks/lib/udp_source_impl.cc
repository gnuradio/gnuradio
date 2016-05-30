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
#include <gnuradio/prefs.h>
#include <stdexcept>
#include <errno.h>
#include <stdio.h>
#include <string.h>

namespace gr {
  namespace blocks {

    const int udp_source_impl::BUF_SIZE_PAYLOADS =
        gr::prefs::singleton()->get_long("udp_blocks", "buf_size_payloads", 50);

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
        d_eof(eof), d_connected(false), d_residual(0), d_sent(0), d_offset(0)
    {
      // Give us some more room to play.
      d_rxbuf = new char[4*d_payload_size];
      d_residbuf = new char[BUF_SIZE_PAYLOADS*d_payload_size];

      connect(host, port);
    }

    udp_source_impl::~udp_source_impl()
    {
      if(d_connected)
        disconnect();

      delete [] d_rxbuf;
      delete [] d_residbuf;
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
      d_socket->async_receive_from(boost::asio::buffer((void*)d_rxbuf, d_payload_size), d_endpoint_rcvd,
                                   boost::bind(&udp_source_impl::handle_read, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred));
    }

    void
    udp_source_impl::handle_read(const boost::system::error_code& error,
                                 size_t bytes_transferred)
    {
      if(!error) {
        {
          boost::lock_guard<gr::thread::mutex> lock(d_udp_mutex);
          if(d_eof && (bytes_transferred == 0)) {
            // If we are using EOF notification, test for it and don't
            // add anything to the output.
            d_residual = WORK_DONE;
            d_cond_wait.notify_one();
            return;
          }
          else {
            // Make sure we never go beyond the boundary of the
            // residual buffer.  This will just drop the last bit of
            // data in the buffer if we've run out of room.
            if((int)(d_residual + bytes_transferred) >= (BUF_SIZE_PAYLOADS*d_payload_size)) {
              GR_LOG_WARN(d_logger, "Too much data; dropping packet.");
            }
            else {
              // otherwise, copy received data into local buffer for
              // copying later.
              memcpy(d_residbuf+d_residual, d_rxbuf, bytes_transferred);
              d_residual += bytes_transferred;
            }
          }
          d_cond_wait.notify_one();
        }
      }
      start_receive();
    }

    int
    udp_source_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock l(d_setlock);

      char *out = (char*)output_items[0];

      // Use async receive_from to get data from UDP buffer and wait
      // on a conditional signal before proceeding. We use this
      // because the conditional wait is interruptable while a
      // synchronous receive_from is not.
      boost::unique_lock<boost::mutex> lock(d_udp_mutex);

      //use timed_wait to avoid permanent blocking in the work function
      d_cond_wait.timed_wait(lock, boost::posix_time::milliseconds(10));

      if (d_residual < 0) {
        return d_residual;
      }

      int bytes_left_in_buffer = (int)(d_residual - d_sent);
      int bytes_to_send        = std::min<int>(d_itemsize * noutput_items, bytes_left_in_buffer);

      // Copy the received data in the residual buffer to the output stream
      memcpy(out, d_residbuf+d_sent, bytes_to_send);
      int nitems = bytes_to_send/d_itemsize;

      // Keep track of where we are if we don't have enough output
      // space to send all the data in the residbuf.
      if (bytes_to_send == bytes_left_in_buffer) {
        d_residual = 0;
        d_sent = 0;
      }
      else {
        d_sent += bytes_to_send;
      }

      return nitems;
    }

  } /* namespace blocks */
} /* namespace gr */
