/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_UHD_AMSG_SOURCE_H
#define INCLUDED_GR_UHD_AMSG_SOURCE_H

#include <gnuradio/msg_queue.h>
#include <gnuradio/uhd/api.h>
#include <uhd/usrp/multi_usrp.hpp>

namespace gr {
namespace uhd {

class uhd_amsg_source;

class GR_UHD_API amsg_source
{
public:
    // gr::uhd::amsg_source::sptr
    typedef boost::shared_ptr<amsg_source> sptr;

    /*!
     * \brief Destructor
     *
     * This must be explicitly defined because there is no block inheritance
     * for this class, and thus no virtual destructor pulled into the class
     * hierarchy for sub-classes.
     */
    virtual ~amsg_source() {}

    /*!
     * \brief Make a new USRP asynchronous message-based source block.
     * \ingroup uhd_blk
     */
    static sptr make(const ::uhd::device_addr_t& device_addr, msg_queue::sptr msgq);

    /*!
     * Convert a raw asynchronous message to an asynchronous metadata object.
     * \return The asynchronous metadata object.
     */
    static ::uhd::async_metadata_t msg_to_async_metadata_t(const message::sptr msg);
};

} /* namespace uhd */
} /* namespace gr */

#endif /* INCLUDED_GR_UHD_AMSG_SOURCE_H */
