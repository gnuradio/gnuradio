/* -*- c++ -*- */
/*
 * Copyright 2025 Skandalis Georgios
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_TCP_SOURCE_H
#define INCLUDED_NETWORK_TCP_SOURCE_H

#include <gnuradio/network/api.h>
#include <gnuradio/sync_block.h>

#define TCP_SOURCE_MODE_CLIENT 1
#define TCP_SOURCE_MODE_SERVER 2

namespace gr {
namespace network {

/*!
 * \brief This block provides a TCP Source block that supports
 * both client and server modes.
 * \ingroup networking_tools
 *
 * \details
 * This block provides a TCP source that supports both listening for
 * inbound connections (server mode) and connecting to other applications
 * (client mode) in order to send data from a GNU Radio flowgraph.
 * The block supports both IPv4 and IPv6 with appropriate code determined
 * by the address used. In server mode, if a client disconnects, the
 * flowgraph will block until a new connection is established.  If/when a new client
 * connection is established, data will then pick up with the current stream for
 * transmission to the new client.
 */
class NETWORK_API tcp_source : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<tcp_source> sptr;

    /*!
     * Build a tcp_source block.
     */
    static sptr make(size_t itemsize,
                     size_t veclen,
                     const std::string& host,
                     const std::string& port,
                     int source_mode);
};

} // namespace network
} // namespace gr

#endif /* INCLUDED_NETWORK_TCP_SOURCE_H */
