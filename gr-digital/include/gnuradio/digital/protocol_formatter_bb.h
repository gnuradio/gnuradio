/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_DIGITAL_PROTOCOL_FORMATTER_BB_H
#define INCLUDED_DIGITAL_PROTOCOL_FORMATTER_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/header_format_base.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Uses a header format object to create a header from a
 * tagged stream packet.
 *
 * \ingroup packet_operators_blk
 *
 * \details
 * This block takes in tagged stream and creates a header,
 * generally for MAC-level processing. Each received tagged stream
 * is assumed to be its own frame, so any fragmentation would be
 * done upstream in or before the flowgraph.
 *
 * The header that is created and transmitted from this block. The
 * payload should then be sent as a parallel tagged stream to be
 * muxed together later. The header is based entirely on the \p
 * format object, which is an object derived from the
 * header_format_base class. All of these packet header format
 * objects operate the same: they take in the payload data as well
 * as possible extra metadata info about the PDU; the format
 * object then returns the output and metadata info. This block
 * then transmits the header vector and attaches and metadata as
 * tags at the start of the header.
 *
 * \sa protocol_formatter_async
 */
class DIGITAL_API protocol_formatter_bb : virtual public tagged_stream_block
{
public:
    typedef boost::shared_ptr<protocol_formatter_bb> sptr;

    /*!
     * Make a packet header block using a given \p format.
     *
     * \param format The format object to use when creating the
     *        header for the packet. Derived from the
     *        header_format_base class.
     * \param len_tag_key The tagged stream length key.
     */
    static sptr make(const header_format_base::sptr& format,
                     const std::string& len_tag_key = "packet_len");

    virtual void set_header_format(header_format_base::sptr& format) = 0;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PROTOCOL_FORMATTER_BB_H */
