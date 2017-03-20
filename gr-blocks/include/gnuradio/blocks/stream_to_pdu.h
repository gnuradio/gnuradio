/* -*- c++ -*- */
/* 
 * Copyright 2017 Free Software Foundation, Inc.
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


#ifndef INCLUDED_BLOCKS_STREAM_TO_PDU_H
#define INCLUDED_BLOCKS_STREAM_TO_PDU_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/blocks/pdu.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Slices an item stream into a PDU of fixed or variable length,
     * triggered by a tag key
     * \ingroup blocks
     *
     * \details
     * This block slices an item stream triggered by a specific tag key and
     * packages the slice into a PDU. The packet length is either
     * fixed (fixed_mode true) or equal to the tag's value (fixed_mode false).
     * The absolute offset of the first sample of the slice is recorded in the PDU
     * metadata dictionary with key offset_key. Any remaining tags that reside within
     * the packet whose keys are not tag_key are added to the PDU metadata dictionary.
     *
     * NOTE: Slices cannot overlap. If a tag exists at offset 100 with length 20
     * and another tag exists at offset 110 with length 50, the second tag at offset 
     * 110 will be ignored. This is to avoid boundless overlap of packets. This block,
     * therefore, will only track one PDU at a time.
     */
    class BLOCKS_API stream_to_pdu : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<stream_to_pdu> sptr;

      /*!
       * \param type PDU type of pdu::vector_type
       * \param fixed_mode Boolean indicating if the PDU length is fixed (true) or variable based
       * on the tag's value (false)
       * \param tag_key The trigger tag's key represented as a string
       * \param offset_key The key in the PDU'a metadata dictionary to hold the uint64
       * absolute offset of the packet's first item
       * \param packet_length The fixed PDU packet length, only applies if fixed_mode is true
       */
      static sptr make(pdu::vector_type type, bool fixed_mode, const std::string& tag_key, const std::string& offset_key, int packet_length = 1);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_STREAM_TO_PDU_H */

