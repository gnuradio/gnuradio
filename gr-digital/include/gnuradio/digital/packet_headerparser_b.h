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

#ifndef INCLUDED_DIGITAL_PACKET_HEADERPARSER_B_H
#define INCLUDED_DIGITAL_PACKET_HEADERPARSER_B_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/digital/packet_header_default.h>

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
      static sptr make(const gr::digital::packet_header_default::sptr &header_formatter);

      /*!
       * \param header_len Number of bytes per header
       * \param len_tag_key Length Tag Key
       */
      static sptr make(long header_len, const std::string &len_tag_key);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_HEADERPARSER_B_H */

