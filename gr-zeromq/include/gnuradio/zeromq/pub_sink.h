/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_PUB_SINK_H
#define INCLUDED_ZEROMQ_PUB_SINK_H

#include <gnuradio/sync_block.h>
#include <gnuradio/zeromq/api.h>

namespace gr {
namespace zeromq {

/*!
 * \brief Sink the contents of a stream to a ZMQ PUB socket
 * \ingroup zeromq
 *
 * \details
 * This block acts as a streaming sink for a GNU Radio flowgraph
 * and writes its contents to a ZMQ PUB socket. A PUB socket may
 * have subscribers and will pass all incoming stream data to each
 * subscriber with a matching key. If the publisher's key is set to
 * "GNURadio", the following example subscriber keys will match: "G",
 * "GN", .., "GNURadio". In other words, the subscriber must contain
 * the first set of characters from the publisher's key. If the subscriber
 * sets an empty key "", it will accept all input messages from the
 * publisher (including the key itself if one is set). Subscribers
 * can either be another gr-zeromq source block or a non-GNU Radio
 * ZMQ socket.
 */
class ZEROMQ_API pub_sink : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<pub_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of zeromq::pub_sink.
     *
     * \param itemsize Size of a stream item in bytes.
     * \param vlen Vector length of the input items. Note that one vector is one item.
     * \param address  ZMQ socket address specifier.
     * \param timeout  Receive timeout in milliseconds, default is 100ms, 1us increments.
     * \param pass_tags Whether sink will serialize and pass tags over the link.
     * \param hwm High Watermark to configure the socket to (-1 => zmq's default)
     * \param key Prepend a key/topic to the start of each message (default is none)
     */
    static sptr make(size_t itemsize,
                     size_t vlen,
                     char* address,
                     int timeout = 100,
                     bool pass_tags = false,
                     int hwm = -1,
                     const std::string& key = "",
                     bool drop_on_hwm = true);

    /*!
     * \brief Return a std::string of ZMQ_LAST_ENDPOINT from the underlying ZMQ socket.
     */
    virtual std::string last_endpoint() = 0;
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_PUB_SINK_H */
