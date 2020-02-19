/* -*- c++ -*- */
/*
 * Copyright 2007-2010,2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_UDP_SOURCE_H
#define INCLUDED_GR_UDP_SOURCE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Read stream from an UDP socket.
 * \ingroup networking_tools_blk
 */
class BLOCKS_API udp_source : virtual public sync_block
{
public:
    // gr::blocks::udp_source::sptr
    typedef boost::shared_ptr<udp_source> sptr;

    /*!
     * \brief UDP Source Constructor
     *
     * \param itemsize     The size (in bytes) of the item datatype
     * \param host         The name or IP address of the transmitting host; can be
     *                     NULL, None, or "0.0.0.0" to allow reading from any
     *                     interface on the host
     * \param port         The port number on which to receive data; use 0 to
     *                     have the system assign an unused port number
     * \param payload_size UDP payload size by default set to 1472 =
     *                     (1500 MTU - (8 byte UDP header) - (20 byte IP header))
     * \param eof          Interpret zero-length packet as EOF (default: true)
     */
    static sptr make(size_t itemsize,
                     const std::string& host,
                     int port,
                     int payload_size = 1472,
                     bool eof = true);

    /*! \brief Change the connection to a new destination
     *
     * \param host         The name or IP address of the receiving host; use
     *                     NULL or None to break the connection without closing
     * \param port         Destination port to connect to on receiving host
     *
     * Calls disconnect() to terminate any current connection first.
     */
    virtual void connect(const std::string& host, int port) = 0;

    /*! \brief Cut the connection if we have one set up.
     */
    virtual void disconnect() = 0;

    /*! \brief return the PAYLOAD_SIZE of the socket */
    virtual int payload_size() = 0;

    /*! \brief return the port number of the socket */
    virtual int get_port() = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_UDP_SOURCE_H */
