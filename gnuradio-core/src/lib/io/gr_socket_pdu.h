/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SOCKET_PDU_H
#define INCLUDED_GR_SOCKET_PDU_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>
#include <gr_stream_pdu_base.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>

class gr_socket_pdu;
typedef boost::shared_ptr<gr_socket_pdu> gr_socket_pdu_sptr;

GR_CORE_API gr_socket_pdu_sptr gr_make_socket_pdu (std::string type, std::string addr, std::string port, int MTU=10000);

class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection>
{
public:
  typedef boost::shared_ptr<tcp_connection> pointer;
  gr_socket_pdu *d_block;
  boost::array<char, 10000> buf;

  static pointer create(boost::asio::io_service& io_service)
  {
    return pointer(new tcp_connection(io_service));
  }

  boost::asio::ip::tcp::socket& socket()
  {
    return socket_;
  }

  void start(gr_socket_pdu* parent)
  {
    d_block = parent;
//    message_ = "connected to gr_socket_pdu\n";
//    boost::asio::async_write(socket_, boost::asio::buffer(message_),
//        boost::bind(&tcp_connection::handle_write, shared_from_this(),
//          boost::asio::placeholders::error,
//          boost::asio::placeholders::bytes_transferred));

    socket_.async_read_some(
        boost::asio::buffer(buf),
         boost::bind(&tcp_connection::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  }
  void send(pmt::pmt_t vector){
    size_t len = pmt::pmt_length(vector);
    size_t offset(0);
    boost::array<char, 10000> txbuf;
    memcpy(&txbuf[0], pmt::pmt_uniform_vector_elements(vector, offset), len);
    boost::asio::async_write(socket_, boost::asio::buffer(txbuf, len),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    }

  ~tcp_connection(){
//    std::cout << "tcp_connection destroyed\n";
    }

private:
  tcp_connection(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

  void handle_read(const boost::system::error_code& error/*error*/, size_t bytes_transferred);

  void handle_write(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/)
  {
  }

  boost::asio::ip::tcp::socket socket_;
  std::string message_;
};


/*!
 * \brief Gather received items into messages and insert into msgq
 * \ingroup sink_blk
 */
class GR_CORE_API gr_socket_pdu : public gr_stream_pdu_base
{
 private:
  friend GR_CORE_API gr_socket_pdu_sptr
  gr_make_socket_pdu(std::string type, std::string addr, std::string port, int MTU);

  boost::asio::io_service _io_service;
 
  boost::array<char, 10000> rxbuf;

  // tcp specific
  boost::asio::ip::tcp::endpoint _tcp_endpoint;

  // specific to tcp server
  boost::shared_ptr<boost::asio::ip::tcp::acceptor> _acceptor_tcp;
  std::vector<tcp_connection::pointer> d_tcp_connections;
  void tcp_server_send(pmt::pmt_t msg);
  void tcp_client_send(pmt::pmt_t msg);
  void udp_send(pmt::pmt_t msg);

  // specific to tcp client
  boost::shared_ptr<boost::asio::ip::tcp::socket> _tcp_socket;

  // specific to udp client/server
  boost::asio::ip::udp::endpoint _udp_endpoint;
  boost::asio::ip::udp::endpoint _udp_endpoint_other;
  boost::shared_ptr<boost::asio::ip::udp::socket> _udp_socket;

  void handle_receive(const boost::system::error_code& error, std::size_t ){
    }
 
  void start_tcp_accept(){
    tcp_connection::pointer new_connection =
        tcp_connection::create(_acceptor_tcp->get_io_service());

    _acceptor_tcp->async_accept(new_connection->socket(),
        boost::bind(&gr_socket_pdu::handle_tcp_accept, this, new_connection,
            boost::asio::placeholders::error));
    }

  void handle_tcp_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error){
        if (!error)
        {
            new_connection->start(this);
            d_tcp_connections.push_back(new_connection);
            start_tcp_accept();
        } else {
            std::cout << error << std::endl;
        }
    }
 
  void run_io_service(){
    _io_service.run();
    } 

  void handle_udp_read(const boost::system::error_code& error/*error*/, size_t bytes_transferred){
    if(!error){
        pmt::pmt_t vector = pmt::pmt_init_u8vector(bytes_transferred, (const uint8_t*)&rxbuf[0]);
        pmt::pmt_t pdu = pmt::pmt_cons( pmt::PMT_NIL, vector);
        
        message_port_pub( pmt::mp("pdus"), pdu );
    
        _udp_socket->async_receive_from( boost::asio::buffer(rxbuf), _udp_endpoint_other,
            boost::bind(&gr_socket_pdu::handle_udp_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    } else {
        throw boost::system::system_error(error);
//        std::cout << "error occurred\n";
    }
  }
  void handle_tcp_read(const boost::system::error_code& error/*error*/, size_t bytes_transferred){
    if(!error)
    {
        pmt::pmt_t vector = pmt::pmt_init_u8vector(bytes_transferred, (const uint8_t*)&rxbuf[0]);
        pmt::pmt_t pdu = pmt::pmt_cons( pmt::PMT_NIL, vector);

        message_port_pub( pmt::mp("pdus"), pdu );

        _tcp_socket->async_read_some(
            boost::asio::buffer(rxbuf),
            boost::bind(&gr_socket_pdu::handle_tcp_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    } else {
        //std::cout << "error occurred\n";
        throw boost::system::system_error(error);
    }
  }

 protected:
  gr_socket_pdu (std::string type, std::string addr, std::string port, int MTU=10000);
 public:
  ~gr_socket_pdu () {}
};

#endif /* INCLUDED_GR_TUNTAP_PDU_H */
