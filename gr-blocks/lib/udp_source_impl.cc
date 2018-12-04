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
        d_eof(eof), d_connected(false)
    {
      // Give us some more room to play.
      connect(host, port);
    }

    udp_source_impl::~udp_source_impl()
    {
      if(d_connected)
        {
          this->disconnect();
        }
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

        boost::asio::ip::udp::resolver::iterator results(resolver.resolve(query)), end;
        while (results != end){
          d_endpoints.emplace_back(*results);
          results++;
        }
        for (const auto& ep: d_endpoints){
          auto sock = std::unique_ptr<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket(d_io_service, ep.protocol()));
          if (ep.protocol() == boost::asio::ip::udp::v6()){
            sock->set_option(boost::asio::ip::v6_only(true));
          }
          sock->set_option(boost::asio::socket_base::reuse_address(true));
          sock->bind(ep);
          d_sockets.push_back(std::move(sock));
          d_rxbufs.emplace_back(std::unique_ptr<char[]>(new char[4*d_payload_size]));
          d_residbufs.emplace_back(std::unique_ptr<char[]>(new char[BUF_SIZE_PAYLOADS*d_payload_size]));
          d_endpoints_rcvd.emplace_back();
          d_residuals.emplace_back(0);
          d_sents.emplace_back(0);
        }
        start_receive();
        d_udp_thread = gr::thread::thread(boost::bind(&udp_source_impl::run_io_service, this));
        d_connected = true;
      }
    }

    void
    udp_source_impl::disconnect()
    {
      gr::thread::scoped_lock lock(d_setlock);

      if(!d_connected){
        return;
      }

      d_io_service.reset();
      d_io_service.stop();
      d_udp_thread.join();

      for (const auto& sock : this->d_sockets){
        sock->close();
      }
      d_connected = false;
    }

    // Return port number of d_socket
    int
    udp_source_impl::get_port(void)
    {
      //return d_endpoint.port();
      return d_sockets.front()->local_endpoint().port();
    }

    void
    udp_source_impl::start_receive()
    {
      for (size_t sock_id = 0; sock_id < this->d_sockets.size(); sock_id++){
        d_sockets[sock_id]->async_receive_from(boost::asio::buffer((void*)&d_rxbufs[sock_id][0], d_payload_size), d_endpoints_rcvd[sock_id],
                                 boost::bind(&udp_source_impl::handle_read, this,
                                             sock_id,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
      }
    }

    void
    udp_source_impl::handle_read(size_t socket_id,
                                 const boost::system::error_code& error,
                                 size_t bytes_transferred)
    {
      if(!error) {
        {
          boost::lock_guard<gr::thread::mutex> lock(d_udp_mutex);
          if(d_eof && (bytes_transferred == 0)) {
            // If we are using EOF notification, test for it and don't
            // add anything to the output.
            d_residuals[socket_id] = WORK_DONE;
            d_cond_wait.notify_one();
            return;
          }
          else {
            // Make sure we never go beyond the boundary of the
            // residual buffer.  This will just drop the last bit of
            // data in the buffer if we've run out of room.
            if((int)(d_residuals[socket_id] + bytes_transferred) >= (BUF_SIZE_PAYLOADS*d_payload_size)) {
              GR_LOG_WARN(d_logger, "Too much data; dropping packet.");
            }
            else {
              // otherwise, copy received data into local buffer for
              // copying later.
              memcpy(&d_residbufs[socket_id][0]+d_residuals[socket_id], &d_rxbufs[socket_id][0], bytes_transferred);
              d_residuals[socket_id] += bytes_transferred;
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

      for (const auto& res : d_residuals)
        {
          if (res < 0) {
            return res;
          }
        }

      int nitems = 0;
      for (size_t sock_id = 0; sock_id < d_sockets.size(); sock_id++){
        int bytes_left_in_buffer = (int)(d_residuals[sock_id] - d_sents[sock_id]);
        int bytes_to_send        = std::min<int>(d_itemsize * noutput_items, bytes_left_in_buffer);

        // Copy the received data in the residual buffer to the output stream
        memcpy(out, &d_residbufs[sock_id][0]+d_sents[sock_id], bytes_to_send);
        nitems += bytes_to_send/d_itemsize;
        noutput_items -= bytes_to_send/d_itemsize;

        // Keep track of where we are if we don't have enough output
        // space to send all the data in the residbuf.
        if (bytes_to_send == bytes_left_in_buffer) {
          d_residuals[sock_id] = 0;
          d_sents[sock_id] = 0;
        }
        else {
          d_sents[sock_id] += bytes_to_send;
        }
      }

      return nitems;
    }

  } /* namespace blocks */
} /* namespace gr */
