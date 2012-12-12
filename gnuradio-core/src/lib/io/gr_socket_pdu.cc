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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_socket_pdu.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <iostream>
#include <gr_pdu.h>
#include <boost/format.hpp>

// public constructor that returns a shared_ptr
gr_socket_pdu_sptr
gr_make_socket_pdu (std::string type, std::string addr, std::string port, int MTU)
{
  return gnuradio::get_initial_sptr(new gr_socket_pdu(type,addr,port,MTU));
}

gr_socket_pdu::gr_socket_pdu (std::string type, std::string addr, std::string port, int MTU)
    : gr_stream_pdu_base(MTU)
{

    if( (type == "TCP_SERVER") || (type == "TCP_CLIENT")){
        boost::asio::ip::tcp::resolver resolver(_io_service);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), addr, port);
        _tcp_endpoint = *resolver.resolve(query);
    }
    if( (type == "UDP_SERVER") || (type == "UDP_CLIENT")){
        boost::asio::ip::udp::resolver resolver(_io_service);
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), addr, port);
        if( (type == "UDP_SERVER") ){
            _udp_endpoint = *resolver.resolve(query);
        } else {
            _udp_endpoint_other = *resolver.resolve(query);
        }
    }

    // register ports
    message_port_register_out(pmt::mp("pdus"));
    message_port_register_in(pmt::mp("pdus"));

    // set up socketry
    if (type == "TCP_SERVER"){
        _acceptor_tcp.reset(new boost::asio::ip::tcp::acceptor(_io_service, _tcp_endpoint));
        _acceptor_tcp->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        start_tcp_accept();
        // bind tcp server send handler
        set_msg_handler(pmt::mp("pdus"), boost::bind(&gr_socket_pdu::tcp_server_send, this, _1));
    } else if(type =="TCP_CLIENT"){
        boost::system::error_code error = boost::asio::error::host_not_found;
        _tcp_socket.reset(new boost::asio::ip::tcp::socket(_io_service));
        _tcp_socket->connect(_tcp_endpoint, error);
        if(error){
            throw boost::system::system_error(error);
        }
        set_msg_handler(pmt::mp("pdus"), boost::bind(&gr_socket_pdu::tcp_client_send, this, _1));
        _tcp_socket->async_read_some(
            boost::asio::buffer(rxbuf),
            boost::bind(&gr_socket_pdu::handle_tcp_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    } else if(type =="UDP_SERVER"){
        _udp_socket.reset(new boost::asio::ip::udp::socket(_io_service, _udp_endpoint));
        _udp_socket->async_receive_from( boost::asio::buffer(rxbuf), _udp_endpoint_other, 
            boost::bind(&gr_socket_pdu::handle_udp_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred)); 
        set_msg_handler(pmt::mp("pdus"), boost::bind(&gr_socket_pdu::udp_send, this, _1));
    } else if(type =="UDP_CLIENT"){
        _udp_socket.reset(new boost::asio::ip::udp::socket(_io_service, _udp_endpoint));
        _udp_socket->async_receive_from( boost::asio::buffer(rxbuf), _udp_endpoint_other, 
            boost::bind(&gr_socket_pdu::handle_udp_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred)); 
        set_msg_handler(pmt::mp("pdus"), boost::bind(&gr_socket_pdu::udp_send, this, _1));
    } else {
        throw std::runtime_error("unknown socket type!");
    }
    
    // start thread for io_service
    d_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&gr_socket_pdu::run_io_service, this)));
    d_started = true;
}

void tcp_connection::handle_read(const boost::system::error_code& error/*error*/, size_t bytes_transferred)
  {
    if(!error)
    {
        pmt::pmt_t vector = pmt::pmt_init_u8vector(bytes_transferred, (const uint8_t*)&buf[0]);
        pmt::pmt_t pdu = pmt::pmt_cons( pmt::PMT_NIL, vector);

        d_block->message_port_pub( pmt::mp("pdus"), pdu );

        socket_.async_read_some(
            boost::asio::buffer(buf),
            boost::bind(&tcp_connection::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    } else {
        std::cout << "error occurred\n";
    }

  }


void gr_socket_pdu::tcp_server_send(pmt::pmt_t msg){
    pmt::pmt_t vector = pmt::pmt_cdr(msg);
    for(size_t i=0; i<d_tcp_connections.size(); i++){
        d_tcp_connections[i]->send(vector);
    }
}

void gr_socket_pdu::tcp_client_send(pmt::pmt_t msg){
    pmt::pmt_t vector = pmt::pmt_cdr(msg);
    size_t len = pmt::pmt_length(vector);
    size_t offset(0);
    boost::array<char, 10000> txbuf;
    memcpy(&txbuf[0], pmt::pmt_uniform_vector_elements(vector, offset), len);
    _tcp_socket->send(boost::asio::buffer(txbuf,len));
}

void gr_socket_pdu::udp_send(pmt::pmt_t msg){
    pmt::pmt_t vector = pmt::pmt_cdr(msg);
    size_t len = pmt::pmt_length(vector);
    size_t offset(0);
    boost::array<char, 10000> txbuf;
    memcpy(&txbuf[0], pmt::pmt_uniform_vector_elements(vector, offset), len);
    if(_udp_endpoint_other.address().to_string() != "0.0.0.0")
        _udp_socket->send_to(boost::asio::buffer(txbuf,len), _udp_endpoint_other);
}
