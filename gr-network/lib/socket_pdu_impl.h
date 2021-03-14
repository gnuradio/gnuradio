/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_SOCKET_PDU_IMPL_H
#define INCLUDED_NETWORK_SOCKET_PDU_IMPL_H

#include "tcp_connection.h"
#include <gnuradio/network/socket_pdu.h>

namespace gr {
namespace network {

class socket_pdu_impl : public socket_pdu
{
private:
    boost::asio::io_service d_io_service;
    std::vector<char> d_rxbuf;
    void run_io_service() { d_io_service.run(); }
    gr::thread::thread d_thread;
    bool d_started;

    // TCP specific
    boost::asio::ip::tcp::endpoint d_tcp_endpoint;
    std::vector<tcp_connection::sptr> d_tcp_connections;
    void handle_tcp_read(const boost::system::error_code& error,
                         size_t bytes_transferred);
    const bool d_tcp_no_delay;

    // TCP server specific
    std::shared_ptr<boost::asio::ip::tcp::acceptor> d_acceptor_tcp;
    void start_tcp_accept();
    void tcp_server_send(pmt::pmt_t msg);
    void handle_tcp_accept(tcp_connection::sptr new_connection,
                           const boost::system::error_code& error);

    // TCP client specific
    std::shared_ptr<boost::asio::ip::tcp::socket> d_tcp_socket;
    void tcp_client_send(pmt::pmt_t msg);

    // UDP specific
    boost::asio::ip::udp::endpoint d_udp_endpoint;
    boost::asio::ip::udp::endpoint d_udp_endpoint_other;
    std::shared_ptr<boost::asio::ip::udp::socket> d_udp_socket;
    void handle_udp_read(const boost::system::error_code& error,
                         size_t bytes_transferred);
    void udp_send(pmt::pmt_t msg);

public:
    socket_pdu_impl(std::string type,
                    std::string addr,
                    std::string port,
                    int MTU = 10000,
                    bool tcp_no_delay = false);
    ~socket_pdu_impl() override;
    bool stop() override;
};

} /* namespace network */
} /* namespace gr */

#endif /* INCLUDED_NETWORK_SOCKET_PDU_IMPL_H */
