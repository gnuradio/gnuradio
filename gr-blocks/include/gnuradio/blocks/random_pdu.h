/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_RANDOM_PDU_H
#define INCLUDED_BLOCKS_RANDOM_PDU_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief Sends a random PDU at intervals
 * \ingroup message_tools_blk
 * \ingroup debug_tools_blk
 */
class BLOCKS_API random_pdu : virtual public block
{
public:
    // gr::blocks::random_pdu::sptr
    typedef boost::shared_ptr<random_pdu> sptr;

    /*!
     * \brief Construct a random PDU generator
     */
    static sptr
    make(int mintime, int maxtime, unsigned char byte_mask = 0xFF, int length_modulo = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_RANDOM_PDU_H */
