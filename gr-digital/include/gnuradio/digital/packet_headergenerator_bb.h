/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PACKET_HEADERGENERATOR_BB_H
#define INCLUDED_PACKET_HEADERGENERATOR_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/tagged_stream_block.h>
#include <gnuradio/digital/packet_header_default.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Generates a header for a tagged, streamed packet.
     * \ingroup packet_operators_blk
     *
     * \details
     * Input: A tagged stream. This is consumed entirely, it is not appended
     *        to the output stream.
     * Output: An tagged stream containing the header. The details on the header
     *         are set in a header formatter object (of type packet_header_default
     *         or a subclass thereof). If only a number of bits is specified, a
     *         default header is generated (see packet_header_default).
     */
    class DIGITAL_API packet_headergenerator_bb : virtual public tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<packet_headergenerator_bb> sptr;

      /* \param header_formatter A header formatter object.
       * \param len_tag_key Length tag key. Note that for header generation,
       *                    it is irrelevant which tag names are set in the
       *                    formatter object, only this value is relevant!
       */
      static sptr make(
	  const packet_header_default::sptr &header_formatter,
	  const std::string &len_tag_key="packet_len"
      );

      /* \param header_len If this value is given, a packet_header_default
       *                   object is used to generate the header. This is
       *                   the number of bits per header.
       * \param len_tag_key Length tag key.
       */
      static sptr make(
	   long header_len,
	   const std::string &len_tag_key = "packet_len");

      virtual void set_header_formatter(packet_header_default::sptr header_formatter)=0;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_PACKET_HEADERGENERATOR_BB_H */

