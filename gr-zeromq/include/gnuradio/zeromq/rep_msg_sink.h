/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_REP_MSG_SINK_H
#define INCLUDED_ZEROMQ_REP_MSG_SINK_H

#include <gnuradio/block.h>
#include <gnuradio/zeromq/api.h>

namespace gr {
namespace zeromq {

/*!
 * \brief Sink the contents of a msg port to a ZMQ REP socket
 * \ingroup zeromq
 *
 * \details
 * This block acts a message port receiver and writes individual
 * messages to a ZMQ REP socket.  The corresponding receiving ZMQ
 * REQ socket can be either another gr-zeromq source block or a
 * non-GNU Radio ZMQ socket.
 */
class ZEROMQ_API rep_msg_sink : virtual public gr::block
{
public:
    typedef boost::shared_ptr<rep_msg_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of zeromq::rep_msg_sink.
     *
     * \param address  ZMQ socket address specifier
     * \param timeout  Receive timeout in milliseconds, default is 100ms, 1us increments
     *
     */
    static sptr make(char* address, int timeout = 100);

    /*!
     * \brief Return a std::string of ZMQ_LAST_ENDPOINT from the underlying ZMQ socket.
     */
    virtual std::string last_endpoint() = 0;
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_REP_MSG_SINK_H */
