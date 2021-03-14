/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tcp_connection.h"
#include <gnuradio/basic_block.h>
#include <gnuradio/pdu.h>

namespace gr {
namespace network {

tcp_connection::sptr tcp_connection::make(boost::asio::io_service& io_service,
                                          int MTU /*= 10000*/,
                                          bool no_delay /*=false*/)
{
    return sptr(new tcp_connection(io_service, MTU, no_delay));
}

tcp_connection::tcp_connection(boost::asio::io_service& io_service,
                               int MTU /*= 10000*/,
                               bool no_delay /*=false*/)
    : d_socket(io_service), d_buf(MTU), d_block(NULL), d_no_delay(no_delay)
{
    try {
        d_socket.set_option(boost::asio::ip::tcp::no_delay(no_delay));
    } catch (...) {
        // Silently ignore failure (socket might be current in accept stage) and try again
        // in 'start'
    }
}

void tcp_connection::send(pmt::pmt_t vector)
{
    size_t len = pmt::blob_length(vector);

    // Asio async_write() requires the buffer to remain valid until the handler is called.
    auto txbuf = std::make_shared<std::vector<char>>(len);

    size_t temp = 0;
    memcpy(txbuf->data(), pmt::uniform_vector_elements(vector, temp), len);

    size_t offset = 0;
    while (offset < len) {
        // Limit the size of each write() to the MTU.
        // FIXME: Note that this has the effect of breaking a large PDU into several
        // smaller PDUs, each containing <= MTU bytes. Is this the desired behavior?
        size_t send_len = std::min((len - offset), d_buf.size());
        boost::asio::async_write(
            d_socket,
            boost::asio::buffer(txbuf->data() + offset, send_len),
            [txbuf](const boost::system::error_code& error, size_t bytes_transferred) {});
        offset += send_len;
    }
}

void tcp_connection::start(gr::basic_block* block)
{
    d_block = block;
    d_socket.set_option(boost::asio::ip::tcp::no_delay(d_no_delay));
    d_socket.async_read_some(boost::asio::buffer(d_buf),
                             boost::bind(&tcp_connection::handle_read,
                                         this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::handle_read(const boost::system::error_code& error,
                                 size_t bytes_transferred)
{
    if (!error) {
        if (d_block) {
            pmt::pmt_t vector =
                pmt::init_u8vector(bytes_transferred, (const uint8_t*)&d_buf[0]);
            pmt::pmt_t pdu = pmt::cons(pmt::PMT_NIL, vector);

            d_block->message_port_pub(msgport_names::pdus(), pdu);
        }

        d_socket.async_read_some(
            boost::asio::buffer(d_buf),
            boost::bind(&tcp_connection::handle_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else {
        d_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        d_socket.close();
    }
}
} /* namespace network */
} /* namespace gr */
