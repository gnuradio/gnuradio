/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_SOCKET_PDU_H
#define INCLUDED_NETWORK_SOCKET_PDU_H

#include <gnuradio/block.h>
#include <gnuradio/network/api.h>

namespace gr {
namespace network {

/*!
 * \brief Creates socket interface and translates traffic to PDUs
 * \ingroup networking_tools_blk
 */
class NETWORK_API socket_pdu : virtual public block
{
public:
    // gr::network::socket_pdu::sptr
    typedef std::shared_ptr<socket_pdu> sptr;

    /*!
     * \brief Construct a SOCKET PDU interface
     * \param type "TCP_SERVER", "TCP_CLIENT", "UDP_SERVER", or "UDP_CLIENT"
     * \param addr network address to use
     * \param port network port to use
     * \param MTU maximum transmission unit
     * \param tcp_no_delay TCP No Delay option (set to True to disable Nagle algorithm)
     */
    static sptr make(std::string type,
                     std::string addr,
                     std::string port,
                     int MTU = 10000,
                     bool tcp_no_delay = false);
};

} /* namespace network */
} /* namespace gr */

#endif /* INCLUDED_NETWORK_SOCKET_PDU_H */
