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

#ifndef INCLUDED_BLOCKS_SOCKET_PDU_IMPL_H
#define INCLUDED_BLOCKS_SOCKET_PDU_IMPL_H

#include <gnuradio/blocks/socket_pdu.h>
#include "tcp_connection.h"

namespace gr {
  namespace blocks {

    class socket_pdu_impl : public socket_pdu
    {
    private:
      boost::asio::io_service d_io_service;
      std::vector<char> d_rxbuf;
      void run_io_service() { d_io_service.run(); }
      gr::thread::thread d_thread;
      bool d_started;
      bool d_finished;

      // TCP specific
      boost::asio::ip::tcp::endpoint d_tcp_endpoint;
      std::vector<tcp_connection::sptr> d_tcp_connections;
      void handle_tcp_read(const boost::system::error_code& error, size_t bytes_transferred);
      bool d_tcp_no_delay;

      // TCP server specific
      boost::shared_ptr<boost::asio::ip::tcp::acceptor> d_acceptor_tcp;
      void start_tcp_accept();
      void tcp_server_send(pmt::pmt_t msg);
      void handle_tcp_accept(tcp_connection::sptr new_connection, const boost::system::error_code& error);

      // TCP client specific
      boost::shared_ptr<boost::asio::ip::tcp::socket> d_tcp_socket;
      void tcp_client_send(pmt::pmt_t msg);

      // UDP specific
      boost::asio::ip::udp::endpoint d_udp_endpoint;
      boost::asio::ip::udp::endpoint d_udp_endpoint_other;
      boost::shared_ptr<boost::asio::ip::udp::socket> d_udp_socket;
      void handle_udp_read(const boost::system::error_code& error, size_t bytes_transferred);
      void udp_send(pmt::pmt_t msg);

    public:
      socket_pdu_impl(std::string type, std::string addr, std::string port, int MTU = 10000, bool tcp_no_delay = false);
      ~socket_pdu_impl();
      bool stop();
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_SOCKET_PDU_IMPL_H */
