/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_DIGITAL_CRC32_BB_H
#define INCLUDED_DIGITAL_CRC32_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Byte-stream CRC block
 * \ingroup packet_operators_blk
 *
 * \details
 * Input: stream of bytes, which form a packet. The first byte of the packet
 * has a tag with key "length" and the value being the number of bytes in the
 * packet.
 *
 * Output: The same bytes as incoming, but trailing a CRC32 of the packet.
 * The tag is re-set to the new length.
 */
class DIGITAL_API crc32_bb : virtual public tagged_stream_block
{
public:
    typedef boost::shared_ptr<crc32_bb> sptr;

    /*!
     * \param check Set to true if you want to check CRC, false to create CRC.
     * \param lengthtagname Length tag key for the tagged stream.
     * \param packed If the data is packed or unpacked bits (default=true).
     */
    static sptr make(bool check = false,
                     const std::string& lengthtagname = "packet_len",
                     bool packed = true);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CRC32_BB_H */
