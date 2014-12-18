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

#ifndef INCLUDED_DIGITAL_PACKET_HEADER_DEFAULT_H
#define INCLUDED_DIGITAL_PACKET_HEADER_DEFAULT_H

#include <gnuradio/tags.h>
#include <gnuradio/digital/api.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/crc.hpp>

namespace gr {
  namespace digital {

    /*!
     * \brief Default header formatter for digital packet transmission.
     * \ingroup packet_operators_blk
     *
     * \details
     * For bursty/packetized digital transmission, packets are usually prepended
     * with a packet header, containing the number of bytes etc.
     * This class is not a block, but a tool to create these packet header.
     *
     * This is a default packet header (see header_formatter()) for a description
     * on the header format). To create other header, derive packet header creator
     * classes from this function.
     *
     * gr::digital::packet_headergenerator_bb uses header generators derived from
     * this class to create packet headers from data streams.
     */
    class DIGITAL_API packet_header_default
      : public boost::enable_shared_from_this<gr::digital::packet_header_default>
    {
     public:
      typedef boost::shared_ptr<packet_header_default> sptr;

      packet_header_default(long header_len,
                            const std::string &len_tag_key="packet_len",
                            const std::string &num_tag_key="packet_num",
                            int bits_per_byte=1);
      virtual ~packet_header_default();

      sptr base() { return shared_from_this(); };
      sptr formatter() { return shared_from_this(); };

      void set_header_num(unsigned header_num) { d_header_number = header_num; };
      long header_len() { return d_header_len; };
      pmt::pmt_t len_tag_key() { return d_len_tag_key; };

      /*!
       * \brief Encodes the header information in the given tags into bits and places them into \p out
       *
       * Uses the following header format:
       * Bits 0-11: The packet length (what was stored in the tag with key \p len_tag_key)
       * Bits 12-23: The header number (counts up everytime this function is called)
       * Bit 24-31: 8-Bit CRC
       * All other bits: Are set to zero
       *
       * If the header length is smaller than 32, bits are simply left out. For this
       * reason, they always start with the LSB.
       *
       * However, it is recommended to stay above 32 Bits, in order to have a working
       * CRC.
       */
      virtual bool header_formatter(long packet_len,
                                    unsigned char *out,
                                    const std::vector<tag_t> &tags=std::vector<tag_t>());

      /*!
       * \brief Inverse function to header_formatter().
       *
       * Reads the bit stream in \p header and writes a corresponding tag into \p tags.
       */
      virtual bool header_parser(
	const unsigned char *header,
	std::vector<tag_t> &tags);

      static sptr make(long header_len,
                       const std::string &len_tag_key="packet_len",
                       const std::string &num_tag_key="packet_num",
                       int bits_per_byte=1);

    protected:
      long d_header_len;
      pmt::pmt_t d_len_tag_key;
      pmt::pmt_t d_num_tag_key;
      int d_bits_per_byte;
      unsigned d_header_number;
      unsigned d_mask;
      boost::crc_optimal<8, 0x07, 0xFF, 0x00, false, false>  d_crc_impl;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_HEADER_DEFAULT_H */
