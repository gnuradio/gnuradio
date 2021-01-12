/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_UDP_SOURCE_H
#define INCLUDED_NETWORK_UDP_SOURCE_H

#include <gnuradio/network/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace network {

/*!
 * \brief This block provides a UDP source block that starts a
 * listener on the specified port and waits for inbound UDP packets.
 * \ingroup networking_tools
 *
 * \details
 * This block provides a UDP source that supports receiving data over
 * a UDP stream from external applications.  A number of header formats
 * are supported including None (raw stream), and other header formats
 * that allow for sequence numbers to be tracked.  This feature allows
 * the flowgraph to be aware of any frames dropped in transit or by
 * its receiving stack.  However, this needs to be appropriately
 * paired with the sending application (it needs to send the same
 * header).  The UDP packet size can also be adjusted
 * to support jumbo frames.  For most networks, 1472 is the correct
 * UDP data packet size that optimizes network transmission.  Adjusting
 * this value without a full understanding of the network implications
 * can create additional network fragmentation and inefficient packet
 * usage so should be avoided.  For networks and endpoints supporting
 * jumbo frames of 9000, 8972 would be the appropriate size
 * (9000 - 28 header bytes).  This block does support IPv4 only or
 * dual stack IPv4/IPv6 listening as an endpoint with an enable
 * IPv6 option that can be set on the block properties page.  It can
 * also be set to source zeros (no signal) in the event no data
 * is being received.
 */
class NETWORK_API udp_source : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<udp_source> sptr;

    /*!
     * Build a udp_source block.
     */
    static sptr make(size_t itemsize,
                     size_t vecLen,
                     int port,
                     int header_type,
                     int payloadsize,
                     bool notify_missed,
                     bool source_zeros,
                     bool ipv6);
};

} // namespace network
} // namespace gr

#endif /* INCLUDED_NETWORK_UDP_SOURCE_H */
