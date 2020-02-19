/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_TAGGED_STREAM_TO_PDU_H
#define INCLUDED_BLOCKS_TAGGED_STREAM_TO_PDU_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/blocks/pdu.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Turns received stream data and tags into PDUs and sends
 * them through a message port.
 * \ingroup message_tools_blk
 *
 * The sent message is a PMT-pair (created by pmt::cons()). The
 * first element is a dictionary containing all the tags. The
 * second is a vector containing the actual data.
 */
class BLOCKS_API tagged_stream_to_pdu : virtual public tagged_stream_block
{
public:
    // gr::blocks::tagged_stream_to_pdu::sptr
    typedef boost::shared_ptr<tagged_stream_to_pdu> sptr;

    /*!
     * \brief Construct a tagged_stream_to_pdu block
     * \param type PDU type of pdu::vector_type
     * \param lengthtagname The name of the tag that specifies
     *        how long the packet is.
     */
    static sptr make(pdu::vector_type type,
                     const std::string& lengthtagname = "packet_len");
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_TAGGED_STREAM_TO_PDU_H */
