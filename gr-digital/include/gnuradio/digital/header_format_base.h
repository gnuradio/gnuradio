/* -*- c++ -*- */
/* Copyright 2016 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_HEADER_FORMAT_BASE_H
#define INCLUDED_DIGITAL_HEADER_FORMAT_BASE_H

#include <pmt/pmt.h>
#include <gnuradio/digital/api.h>
#include <gnuradio/digital/header_buffer.h>
#include <gnuradio/logger.h>
#include <boost/enable_shared_from_this.hpp>

namespace gr {
  namespace digital {

    /*!
     * \brief Base header formatter class.
     * \ingroup packet_operators_blk
     *
     * \details
     *
     * Creates a base class that other packet formatters will inherit
     * from. The child classes create and parse protocol-specific
     * headers. To add a new protocol processing class, create a class
     * that inherits from this and overload the necessary
     * functions. The main functions to overload are:
     *
     * \li header_format_base::format: takes in a payload and
     * creates a header from it.
     *
     * \li header_format_base::parse: receive bits and extract
     * the header info. These are expected to be hard bits (0 or 1)
     * that have either been sliced or gone through an FEC decoder.
     *
     * \li header_format_base::header_nbits: the number of bits
     * in the full header (including an access code).
     *
     * \li header_format_base::header_ok: checks to see if the
     * received header is ok. Since the header often specifies the
     * length of the frame to decode next, it is important that this
     * information be correct.
     *
     * \li header_format_base::header_payload: unpacks the header
     * register (from the class header_buffer) as a set of bits into
     * its component parts of the header. For example, this may find
     * and extract the frame length field as a 16-bit value and/or
     * flags related to the type of modulation and FEC codes used in
     * the frame's payload.
     *
     * Protected functions of this class that the child class should
     * overload include:
     *
     * \li enter_search
     * \li enter_have_sync
     * \li enter_have_header
     *
     * These three function represent the different states of the
     * parsing state machine. Expected behavior is that the protocol
     * has some known word that we are first looking for the identify
     * the start of the frame. The parsing FSM starts in a state to
     * search for the beginning of the header, normally by looking for
     * a known word (i.e., the access code). Then it changes state to
     * read in the full header. We expect that the protocol provides
     * the length of the header for processing, so the parsing looks
     * pulls in the full length of the header. Then it changes state
     * to the "have header" state for checking and processing. The
     * base class provides the basic functionality for this state
     * machine. However, most likely, each child class must manage
     * these states for themselves.
     *
     * This class is specifically designed to work with packets/frames
     * in the asynchronous PDU architecture of GNU Radio. See the
     * packet_format_async block for formatting the headers onto
     * payloads and packet_parse_b block for parsing headers in a
     * receiver.
     *
     * The Packet Format block takes in a PDU and uses a formatter
     * class derived from this class to add a header onto the
     * packet. The Packet Format blocks takes in the PDU, unpacks the
     * message, and passes it to a formatter class' format function,
     * which builds a header based on the payload. The header is
     * passed back and emitted from formatter block as a separate
     * output. The async format block, packet_format_async, has two
     * message output ports. The 'header' port passes the header out
     * as a PDU and the 'payload' passes the payload out as a PDU. The
     * flowgraph can then separately modulate and combine these two
     * pieces in the follow-on processing.
     *
     * The packet_sync_b block uses the formatter class by calling the
     * 'parse' function to parse the received packet headers. This
     * parser block is a sink for the data stream and emits a message
     * from an 'info' port that contains an PMT dictionary of the
     * information in the header. The formatter class determines the
     * dictionary keys.
     *
     * This is the base class for dealing with formatting headers for
     * different protocols and purposes. For other header formatting
     * behaviors, create a child class from here and overload the
     * format, parse, and parsing state machine functions as
     * necessary.
     *
     * \sa header_format_default
     * \sa header_format_counter
     */
    class DIGITAL_API header_format_base
      : public boost::enable_shared_from_this<gr::digital::header_format_base>
    {
     public:
      typedef boost::shared_ptr<header_format_base> sptr;

      header_format_base();
      virtual ~header_format_base();

      sptr base() { return shared_from_this(); };
      sptr formatter() { return shared_from_this(); };

      /*!
       * Function to creates a header. The child classes overload this
       * function to format the header in the protocol-specific way.
       *
       * \param nbytes_in The length (in bytes) of the \p input payload
       * \param input An array of unsigned chars of the packet payload
       * \param output A pmt::u8vector with the new header prepended
       *        onto the input data.
       * \param info A pmt::dict containing meta data and info about
       *        the PDU (generally from the metadata portion of the
       *        input PDU). Data can be extracted from this for the
       *        header formatting or inserted.
       *
       * MUST be overloaded.
       */
      virtual bool format(int nbytes_in,
                          const unsigned char *input,
                          pmt::pmt_t &output,
                          pmt::pmt_t &info) = 0;

      /*!
       * Parses a header. This function is overloaded in the child
       * class, which knows how to convert the incoming hard bits (0's
       * and 1's) back into a packet header.
       *
       * \param nbits_in The number of bits in the input array.
       * \param input The input as hard decision bits.
       * \param info A vector of pmt::dicts to hold any meta data or
       *        info about the PDU. When parsing the header, the
       *        formatter can add info from the header into this dict.
       *        Each packet has a single PMT dictionary of info, so
       *        the vector length is the number of packets received
       *        extracted during one call to this parser function.
       * \param nbits_processed Number of input bits actually
       *        processed; If all goes well, this is nbits_in. A
       *        premature return after a bad header could be less than
       *        this.
       *
       * MUST be overloaded.
       */
      virtual bool parse(int nbits_in,
                         const unsigned char *input,
                         std::vector<pmt::pmt_t> &info,
                         int &nbits_processed) = 0;

      /*!
       * Returns the length of the formatted header in bits.
       * MUST be overloaded.
       */
      virtual size_t header_nbits() const = 0;

      /*!
       * Returns the length of the formatted header in bytes.
       * Auto-calculated from the overloaded header_nbits().
       */
      size_t header_nbytes() const;

    protected:
      enum state_t {STATE_SYNC_SEARCH, STATE_HAVE_SYNC};

      state_t d_state;          //!< state of the state machine
      header_buffer d_hdr_reg;  //!< header_buffer object to hold header bits
      pmt::pmt_t d_info;        //!< info captured from the header

      //! Enter Search state of the state machine to find the access code.
      virtual void enter_search();

      //! Access code found, start getting the header
      virtual void enter_have_sync();

      //! Header found, setup for pulling in the hard decision bits
      virtual void enter_have_header(int payload_len);

      //! Verify that the header is valid
      virtual bool header_ok() = 0;

      /*! Get info from the header; return payload length and package
       *  rest of data in d_info dictionary.
       */
      virtual int header_payload() = 0;

      /*! Used by blocks to access the logger system.
       */
      gr::logger_ptr d_logger;
      gr::logger_ptr d_debug_logger;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HEADER_FORMAT_BASE_H */
