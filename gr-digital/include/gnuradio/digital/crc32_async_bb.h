/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_DIGITAL_CRC32_ASYNC_BB_H
#define INCLUDED_DIGITAL_CRC32_ASYNC_BB_H

#include <gnuradio/block.h>
#include <gnuradio/digital/api.h>

namespace gr {
namespace digital {

/*!
 * \brief Byte-stream CRC block for async messages
 * \ingroup packet_operators_blk
 * \ingroup deprecated_blk
 *
 * \details
 *
 * Processes packets (as async PDU messages) for CRC32. The \p
 * check parameter determines if the block acts to check and strip
 * the CRC or to calculate and append the CRC32.
 *
 * The input PDU is expected to be a message of packet bytes.
 *
 * When using check mode, if the CRC passes, the output is a
 * payload of the message with the CRC stripped, so the output
 * will be 4 bytes smaller than the input.
 *
 * When using calculate mode (check == false), then the CRC is
 * calculated on the PDU and appended to it. The output is then 4
 * bytes longer than the input.
 *
 * This block implements the CRC32 using the standard generator 0x04C11DB7.
 */
class DIGITAL_API crc32_async_bb : virtual public block
{
public:
    typedef std::shared_ptr<crc32_async_bb> sptr;

    /*!
     * \param check Set to true if you want to check CRC, false to create CRC.
     */
    static sptr make(bool check = false);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CRC32_ASYNC_BB_H */
