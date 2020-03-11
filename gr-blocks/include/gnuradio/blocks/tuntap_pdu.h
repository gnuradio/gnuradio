/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_TUNTAP_PDU_H
#define INCLUDED_BLOCKS_TUNTAP_PDU_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief Creates TUNTAP interface and translates traffic to PDUs
 * \ingroup networking_tools_blk
 */
class BLOCKS_API tuntap_pdu : virtual public block
{
public:
    // gr::blocks::tuntap_pdu::sptr
    typedef boost::shared_ptr<tuntap_pdu> sptr;

    /*!
     * \brief Construct a TUNTAP PDU interface
     * \param dev Device name to create
     * \param MTU Maximum Transmission Unit size
     * \param istunflag Flag to indicate TUN or Tap
     */
    static sptr make(std::string dev, int MTU = 10000, bool istunflag = false);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_TUNTAP_PDU_H */
