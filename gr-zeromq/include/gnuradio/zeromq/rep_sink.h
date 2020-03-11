/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_REP_SINK_H
#define INCLUDED_ZEROMQ_REP_SINK_H

#include <gnuradio/sync_block.h>
#include <gnuradio/zeromq/api.h>

namespace gr {
namespace zeromq {

/*!
 * \brief Sink the contents of a stream to a ZMQ REP socket
 * \ingroup zeromq
 *
 * \details
 * This block acts a a streaming sink for a GNU Radio flowgraph
 * and writes its contents to a ZMQ REP socket.  A REP socket will
 * only send its contents to an attached REQ socket when it
 * requests items.
 */
class ZEROMQ_API rep_sink : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<rep_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of zeromq::rep_sink.
     *
     * \param itemsize Size of a stream item in bytes.
     * \param vlen Vector length of the input items. Note that one vector is one item.
     * \param address  ZMQ socket address specifier.
     * \param timeout  Receive timeout in milliseconds, default is 100ms, 1us increments.
     * \param pass_tags Whether sink will serialize and pass tags over the link.
     * \param hwm High Watermark to configure the socket to (-1 => zmq's default)
     */
    static sptr make(size_t itemsize,
                     size_t vlen,
                     char* address,
                     int timeout = 100,
                     bool pass_tags = false,
                     int hwm = -1);

    /*!
     * \brief Return a std::string of ZMQ_LAST_ENDPOINT from the underlying ZMQ socket.
     */
    virtual std::string last_endpoint() = 0;
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_REP_SINK_H */
