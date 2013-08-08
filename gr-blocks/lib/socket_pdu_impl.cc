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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "socket_pdu_impl.h"
#include "tcp_connection.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/blocks/pdu.h>

namespace gr {
  namespace blocks {

    socket_pdu::sptr
    socket_pdu::make(std::string type, std::string addr, std::string port, int MTU)
    {
      return gnuradio::get_initial_sptr(new socket_pdu_impl(type, addr, port, MTU));
    }

    socket_pdu_impl::socket_pdu_impl(std::string type, std::string addr, std::string port, int MTU)
      :	block("socket_pdu",
		 io_signature::make (0, 0, 0),
		 io_signature::make (0, 0, 0))
    {
      message_port_register_in(PDU_PORT_ID);
      message_port_register_out(PDU_PORT_ID);

      if ((type == "TCP_SERVER") || (type == "TCP_CLIENT")) {
        boost::asio::ip::tcp::resolver resolver(d_io_service);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), addr, port);
	d_tcp_endpoint = *resolver.resolve(query);
      }

      if ((type == "UDP_SERVER") || (type == "UDP_CLIENT")) {
	boost::asio::ip::udp::resolver resolver(d_io_service);
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), addr, port);

        if (type == "UDP_SERVER")
	  d_udp_endpoint = *resolver.resolve(query);
        else
	  d_udp_endpoint_other = *resolver.resolve(query);
      }

      if (type == "TCP_SERVER") {
        d_acceptor_tcp.reset(new boost::asio::ip::tcp::acceptor(d_io_service, d_tcp_endpoint));
        d_acceptor_tcp->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        start_tcp_accept();
        set_msg_handler(PDU_PORT_ID, boost::bind(&socket_pdu_impl::tcp_server_send, this, _1));

      }
      else if (type =="TCP_CLIENT") {
        boost::system::error_code error = boost::asio::error::host_not_found;
        d_tcp_socket.reset(new boost::asio::ip::tcp::socket(d_io_service));
        d_tcp_socket->connect(d_tcp_endpoint, error);
        if (error)
            throw boost::system::system_error(error);

        set_msg_handler(PDU_PORT_ID, boost::bind(&socket_pdu_impl::tcp_client_send, this, _1));

        d_tcp_socket->async_read_some(
	  boost::asio::buffer(d_rxbuf),
	  boost::bind(&socket_pdu_impl::handle_tcp_read, this, 
                      boost::asio::placeholders::error, 
                      boost::asio::placeholders::bytes_transferred)
	);
      }
      else if (type =="UDP_SERVER") {
        d_udp_socket.reset(new boost::asio::ip::udp::socket(d_io_service, d_udp_endpoint));
        d_udp_socket->async_receive_from(boost::asio::buffer(d_rxbuf), d_udp_endpoint_other, 
					 boost::bind(&socket_pdu_impl::handle_udp_read, this,
						     boost::asio::placeholders::error,
						     boost::asio::placeholders::bytes_transferred));
        set_msg_handler(PDU_PORT_ID, boost::bind(&socket_pdu_impl::udp_send, this, _1));
      }
      else if (type =="UDP_CLIENT") {
        d_udp_socket.reset(new boost::asio::ip::udp::socket(d_io_service, d_udp_endpoint));
        d_udp_socket->async_receive_from(boost::asio::buffer(d_rxbuf), d_udp_endpoint_other, 
					 boost::bind(&socket_pdu_impl::handle_udp_read, this,
						     boost::asio::placeholders::error,
						     boost::asio::placeholders::bytes_transferred)); 
        set_msg_handler(PDU_PORT_ID, boost::bind(&socket_pdu_impl::udp_send, this, _1));
      }
      else
	throw std::runtime_error("gr::blocks:socket_pdu: unknown socket type");

      d_thread = gr::thread::thread(boost::bind(&socket_pdu_impl::run_io_service, this));
      d_started = true;
    }

    void
    socket_pdu_impl::handle_tcp_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
      if (!error) {
	pmt::pmt_t vector = pmt::init_u8vector(bytes_transferred, (const uint8_t *)&d_rxbuf[0]);
	pmt::pmt_t pdu = pmt::cons(pmt::PMT_NIL, vector);
	message_port_pub(PDU_PORT_ID, pdu);

	d_tcp_socket->async_read_some(boost::asio::buffer(d_rxbuf),
				      boost::bind(&socket_pdu_impl::handle_tcp_read, this,
						  boost::asio::placeholders::error,
						  boost::asio::placeholders::bytes_transferred));
      }
      else
        throw boost::system::system_error(error);
    }

    void
    socket_pdu_impl::start_tcp_accept()
    {
      tcp_connection::sptr new_connection = tcp_connection::make(d_acceptor_tcp->get_io_service());

      d_acceptor_tcp->async_accept(new_connection->socket(),
				   boost::bind(&socket_pdu_impl::handle_tcp_accept, this, 
					       new_connection, boost::asio::placeholders::error));
    }

    void
    socket_pdu_impl::tcp_server_send(pmt::pmt_t msg)
    {
      pmt::pmt_t vector = pmt::cdr(msg);
      for(size_t i = 0; i < d_tcp_connections.size(); i++)
        d_tcp_connections[i]->send(vector);
    }

    void
    socket_pdu_impl::handle_tcp_accept(tcp_connection::sptr new_connection, const boost::system::error_code& error)
    {
      if (!error) {
	new_connection->start(this);
	d_tcp_connections.push_back(new_connection);
	start_tcp_accept();
      }
      else
	std::cout << error << std::endl;
    }

    void
    socket_pdu_impl::tcp_client_send(pmt::pmt_t msg)
    {
      pmt::pmt_t vector = pmt::cdr(msg);
      size_t len = pmt::length(vector);
      size_t offset(0);
      boost::array<char, 10000> txbuf;
      memcpy(&txbuf[0], pmt::uniform_vector_elements(vector, offset), len);
      d_tcp_socket->send(boost::asio::buffer(txbuf,len));
    }

    void
    socket_pdu_impl::udp_send(pmt::pmt_t msg)
    {
      pmt::pmt_t vector = pmt::cdr(msg);
      size_t len = pmt::length(vector);
      size_t offset(0);
      boost::array<char, 10000> txbuf;
      memcpy(&txbuf[0], pmt::uniform_vector_elements(vector, offset), len);
      if (d_udp_endpoint_other.address().to_string() != "0.0.0.0")
        d_udp_socket->send_to(boost::asio::buffer(txbuf,len), d_udp_endpoint_other);
    }

    void
    socket_pdu_impl::handle_udp_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
      if (!error) {
        pmt::pmt_t vector = pmt::init_u8vector(bytes_transferred, (const uint8_t*)&d_rxbuf[0]);
        pmt::pmt_t pdu = pmt::cons( pmt::PMT_NIL, vector);
        
        message_port_pub(PDU_PORT_ID, pdu);
    
        d_udp_socket->async_receive_from(boost::asio::buffer(d_rxbuf), d_udp_endpoint_other,
					 boost::bind(&socket_pdu_impl::handle_udp_read, this,
						     boost::asio::placeholders::error,
						     boost::asio::placeholders::bytes_transferred));
      } 
    }

  } /* namespace blocks */
}/* namespace gr */
