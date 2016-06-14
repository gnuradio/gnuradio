/* -*- c++ -*- */
/*
 * Copyright 2015-2016 Free Software Foundation, Inc.
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


#ifndef INCLUDED_DIGITAL_PACKET_PARSE_B_H
#define INCLUDED_DIGITAL_PACKET_PARSE_B_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/header_format_base.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Block that synchronizes to a header based on a header
     * format object class. Designed to accept hard bits and produce
     * PDUs with packed bytes (pmt::u8vector).
     *
     * \ingroup packet_operators_blk
     *
     * \details
     *
     * A packet synchronizer block. This block takes in hard bits
     * (unpacked bytes; 1's and 0's as the LSB) and finds the access
     * code as a sync word to find the start of a frame.
     *
     * The block uses a format object derived from a
     * header_format_base class.
     *
     * Once the frame is detected (usually through the use of an
     * access code), the block uses the format object's parser
     * function to decode the remaining header. Generally, as in the
     * default header case, the header will contain the length of the
     * frame's payload. That and anything else in the header will
     * generally go into the PDU's meta-data dictionary.
     *
     * The block will output a PDU that contains frame's header info
     * in the meta-data portion of the PDU and the payload itself. The
     * payload is packed hard bits as taken from the input stream.
     *
     * \sa packet_sync_ff for a soft decision version.
     */
    class DIGITAL_API protocol_parser_b : virtual public sync_block
    {
     public:
      typedef boost::shared_ptr<protocol_parser_b> sptr;

      /*!
       * Make a packet header block using a given \p format.
       *
       * \param format The format object to use when reading the
       *        header.
       */
      static sptr make(const header_format_base::sptr &format);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PROTOCOL_PARSER_B_H */
