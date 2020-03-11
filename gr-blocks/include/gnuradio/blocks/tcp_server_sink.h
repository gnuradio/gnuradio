/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_TCP_SERVER_SINK_H
#define INCLUDED_BLOCKS_TCP_SERVER_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Send stream through a TCP socket.
 * \ingroup networking_tools_blk
 *
 * \details
 * Listen for incoming TCP connection(s). Duplicate data for each
 * opened connection. Optionally can wait until first client connects
 * before streaming starts.
 */
class BLOCKS_API tcp_server_sink : virtual public gr::sync_block
{
public:
    // gr::blocks::tcp_server_sink::sptr
    typedef boost::shared_ptr<tcp_server_sink> sptr;

    /*!
     * \brief TCP Server Sink Constructor
     *
     * \param itemsize     The size (in bytes) of the item datatype
     * \param host         The name or IP address of interface to bind to.
     * \param port         Port where to listen.
     * \param noblock      If false, wait until first client connects before
     *                     streaming starts. In non blocking mode
     *                     (noblock=true), drop data onto floor if no client
     *                     is connected.
     */
    static sptr
    make(size_t itemsize, const std::string& host, int port, bool noblock = false);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_TCP_SERVER_SINK_H */
