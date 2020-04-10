/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_PUB_MSG_SINK_H
#define INCLUDED_ZEROMQ_PUB_MSG_SINK_H

#include <gnuradio/block.h>
#include <gnuradio/zeromq/api.h>

namespace gr {
namespace zeromq {

/*!
 * \brief Sink the contents of a msg port to a ZMQ PUB socket
 * \ingroup zeromq
 *
 * \details
 * This block acts a message port receiver and writes individual
 * messages to a ZMQ PUB socket.  A PUB socket may have
 * subscribers and will pass all incoming messages to each
 * subscriber.  Subscribers can be either another gr-zeromq source
 * block or a non-GNU Radio ZMQ socket.
 */
class ZEROMQ_API pub_msg_sink : virtual public gr::block
{
public:
    typedef std::shared_ptr<pub_msg_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of zeromq::pub_msg_sink.
     *
     * \param address  ZMQ socket address specifier
     * \param timeout  Receive timeout in milliseconds, default is 100ms, 1us increments
     * \param bind     If true this block will bind to the address, otherwise it will
     * connect; the default is to bind
     *
     */
    static sptr make(char* address, int timeout = 100, bool bind = true);

    /*!
     * \brief Return a std::string of ZMQ_LAST_ENDPOINT from the underlying ZMQ socket.
     */
    virtual std::string last_endpoint() = 0;
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_PUB_MSG_SINK_H */
