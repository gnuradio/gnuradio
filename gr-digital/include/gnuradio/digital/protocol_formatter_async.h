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


#ifndef INCLUDED_DIGITAL_PROTOCOL_FORMATTER_ASYNC_H
#define INCLUDED_DIGITAL_PROTOCOL_FORMATTER_ASYNC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/header_format_base.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Uses a header format object to append a header onto a
     * PDU.
     *
     * \ingroup packet_operators_blk
     *
     * \details
     * This block takes in PDUs and creates a header, generally for
     * MAC-level processing. Each received PDU is assumed to be its
     * own frame, so any fragmentation would be done upstream in or
     * before the flowgraph.
     *
     * The header that is created and transmitted out of the 'header'
     * message port as a PDU. The header is based entirely on the \p
     * format object, which is an object derived from the
     * header_format_base class. All of these packet header format
     * objects operate the same: they take in the payload data as well
     * as possible extra metadata info about the PDU; the format
     * object then returns the output PDU as a PMT argument along
     * with any changes to the metadata info PMT.
     *
     * For different packet header formatting needs, we can define new
     * classes that inherit from the header_format_base block
     * and which overload the header_format_base::format
     * function.
     *
     * \sa header_format_base
     * \sa header_format_default
     * \sa header_format_counter
     *
     * This block only uses asynchronous message passing interfaces to
     * receiver and emit PDUs. The message ports are:
     *
     * \li in: receives PDUs for the frame payload
     * \li header: the header formatted for the given frame
     * \li payload: the payload
     */
    class DIGITAL_API protocol_formatter_async : virtual public block
    {
     public:
      typedef boost::shared_ptr<protocol_formatter_async> sptr;

      /*!
       * Make a packet header block using a given \p format.
       *
       * \param format The format object to use when creating the
       *        header for the packet.
       */
      static sptr make(const header_format_base::sptr &format);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PROTOCOL_FORMATTER_ASYNC_H */
