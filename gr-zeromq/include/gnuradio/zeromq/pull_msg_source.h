/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_PULL_MSG_SOURCE_H
#define INCLUDED_ZEROMQ_PULL_MSG_SOURCE_H

#include <gnuradio/block.h>
#include <gnuradio/zeromq/api.h>

namespace gr {
namespace zeromq {

/*!
 * \brief Receive messages on ZMQ PULL socket and output async messages
 * \ingroup zeromq
 *
 * \details
 * This block will connect to a ZMQ PUSH socket, then convert
 * received messages to outgoing async messages.
 */
class ZEROMQ_API pull_msg_source : virtual public gr::block
{
public:
    typedef std::shared_ptr<pull_msg_source> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of gr::zeromq::pull_msg_source.
     *
     * \param address  ZMQ socket address specifier
     * \param timeout  Receive timeout in milliseconds, default is 100ms, 1us increments
     * \param bind     If true this block will bind to the address, otherwise it will
     * connect; the default is to connect
     *
     */
    static sptr make(char* address, int timeout = 100, bool bind = false);

    /*!
     * \brief Return a std::string of ZMQ_LAST_ENDPOINT from the underlying ZMQ socket.
     */
    virtual std::string last_endpoint() = 0;
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_PULL_MSG_SOURCE_H */
