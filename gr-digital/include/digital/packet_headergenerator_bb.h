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

#include <digital/api.h>
#include <gr_tagged_stream_block.h>
#include <digital/packet_header_default.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Generates a header for a tagged, streamed packet.
     * \ingroup digital
     *
     * Input: A tagged stream. The first element must have a tag with the key
     *        specified in len_tag_key, which hold the exact number of elements
     *        in the PDU.
     * Output: An tagged stream of length header_len. The output is determined
     *         by a header formatter.
     */
    class DIGITAL_API packet_headergenerator_bb : virtual public gr_tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<packet_headergenerator_bb> sptr;

      static sptr make(const packet_header_default::sptr &header_formatter);
      static sptr make(
	   long header_len,
	   const std::string &len_tag_key = "packet_len");
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_PACKET_HEADERGENERATOR_BB_H */

