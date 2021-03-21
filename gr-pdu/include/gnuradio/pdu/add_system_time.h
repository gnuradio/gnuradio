/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_PDU_ADD_SYSTEM_TIME_H
#define INCLUDED_PDU_ADD_SYSTEM_TIME_H

#include <gnuradio/block.h>
#include <gnuradio/pdu/api.h>

namespace gr {
namespace pdu {

/*!
 * \brief Adds system time to a PDU's metadata
 * \ingroup debug_tools_blk
 * \ingroup pdu_blk
 *
 * Adds a user specified key to PDU dict containing the boost system time in seconds
 * since unix epoch.
 */
class PDU_API add_system_time : virtual public gr::block
{
public:
    typedef std::shared_ptr<add_system_time> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of pdu::add_system_time.
     *
     * @param key - key to use for system time metadata field
     */
    static sptr make(const pmt::pmt_t key);
};

} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_ADD_SYSTEM_TIME_H */
