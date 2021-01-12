/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_TCP_SINK_H
#define INCLUDED_NETWORK_TCP_SINK_H

#include <gnuradio/network/api.h>
#include <gnuradio/sync_block.h>

#define TCPSINKMODE_CLIENT 1
#define TCPSINKMODE_SERVER 2

namespace gr {
namespace network {

/*!
 * \brief This block provides a TCP Sink block that supports
 * both client and server modes.
 * \ingroup networking_tools
 *
 * \details
 * This block provides a TCP sink that supports both listening for
 * inbound connections (server mode) and connecting to other applications
 * (client mode) in order to send data from a GNU Radio flowgraph.
 * The block supports both IPv4 and IPv6 with appropriate code determined
 * by the address used.  In server mode, if a client disconnects, the
 * flowgraph will continue to execute.  If/when a new client connection
 * is established, data will then pick up with the current stream for
 * transmission to the new client.
 */
class NETWORK_API tcp_sink : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<tcp_sink> sptr;

    /*!
     * Build a tcp_sink block.
     */
    static sptr
    make(size_t itemsize, size_t veclen, const std::string& host, int port, int sinkmode);
};

} // namespace network
} // namespace gr

#endif /* INCLUDED_NETWORK_TCP_SINK_H */
