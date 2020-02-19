/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_PACKET_HEADERPARSER_B_H
#define INCLUDED_DIGITAL_PACKET_HEADERPARSER_B_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/packet_header_default.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Post header metadata as a PMT
 * \ingroup packet_operators_blk
 *
 * \details
 * In a sense, this is the inverse block to packet_headergenerator_bb.
 * The difference is, the parsed header is not output as a stream,
 * but as a PMT dictionary, which is published to message port with
 * the id "header_data".
 *
 * The dictionary consists of the tags created by the header formatter
 * object. You should be able to use the exact same formatter object
 * as used on the Tx side in the packet_headergenerator_bb.
 *
 * If only a header length is given, this block uses the default header
 * format.
 */
class DIGITAL_API packet_headerparser_b : virtual public sync_block
{
public:
    typedef boost::shared_ptr<packet_headerparser_b> sptr;

    /*!
     * \param header_formatter Header object. This should be the same as used for
     *                         packet_headergenerator_bb.
     */
    static sptr make(const gr::digital::packet_header_default::sptr& header_formatter);

    /*!
     * \param header_len Number of bytes per header
     * \param len_tag_key Length Tag Key
     */
    static sptr make(long header_len, const std::string& len_tag_key);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_HEADERPARSER_B_H */
