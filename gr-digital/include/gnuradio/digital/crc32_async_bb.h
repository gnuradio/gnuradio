/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_DIGITAL_CRC32_ASYNC_BB_H
#define INCLUDED_DIGITAL_CRC32_ASYNC_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Byte-stream CRC block for async messages
     * \ingroup packet_operators_blk
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
     * This block implements the CRC32 using the Boost crc_optimal
     * class for 32-bit CRCs with the standard generator 0x04C11DB7.
     */
    class DIGITAL_API crc32_async_bb : virtual public block
    {
     public:
      typedef boost::shared_ptr<crc32_async_bb> sptr;

      /*!
       * \param check Set to true if you want to check CRC, false to create CRC.
       */
      static sptr make(bool check=false);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CRC32_ASYNC_BB_H */
