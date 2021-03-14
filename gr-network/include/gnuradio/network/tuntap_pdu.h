/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_TUNTAP_PDU_H
#define INCLUDED_NETWORK_TUNTAP_PDU_H

#include <gnuradio/block.h>
#include <gnuradio/network/api.h>

namespace gr {
namespace network {

/*!
 * \brief Creates TUNTAP interface and translates traffic to PDUs
 * \ingroup networking_tools_blk
 */
class NETWORK_API tuntap_pdu : virtual public block
{
public:
    // gr::network::tuntap_pdu::sptr
    typedef std::shared_ptr<tuntap_pdu> sptr;

    /*!
     * \brief Construct a TUNTAP PDU interface
     * \param dev Device name to create
     * \param MTU Maximum Transmission Unit size
     * \param istunflag Flag to indicate TUN or Tap
     */
    static sptr make(std::string dev, int MTU = 10000, bool istunflag = false);
};

} /* namespace network */
} /* namespace gr */

#endif /* INCLUDED_NETWORK_TUNTAP_PDU_H */
