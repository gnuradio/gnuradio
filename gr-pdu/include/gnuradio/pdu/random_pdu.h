/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_RANDOM_PDU_H
#define INCLUDED_PDU_RANDOM_PDU_H

#include <gnuradio/block.h>
#include <gnuradio/pdu/api.h>

namespace gr {
namespace pdu {

/*!
 * \brief Sends a random PDU at intervals
 * \ingroup message_tools_blk
 * \ingroup debug_tools_blk
 */
class PDU_API random_pdu : virtual public block
{
public:
    // gr::pdu::random_pdu::sptr
    typedef std::shared_ptr<random_pdu> sptr;

    /*!
     * \brief Construct a random PDU generator
     */
    static sptr
    make(int mintime, int maxtime, unsigned char byte_mask = 0xFF, int length_modulo = 1);
};

} /* namespace pdu */
} /* namespace gr */

#endif /* INCLUDED_PDU_RANDOM_PDU_H */
