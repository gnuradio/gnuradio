/* -*- c++ -*- */
/* Copyright 2015-2016 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_HEADER_FORMAT_DEFAULT_H
#define INCLUDED_DIGITAL_HEADER_FORMAT_DEFAULT_H

#include <pmt/pmt.h>
#include <gnuradio/digital/api.h>
#include <gnuradio/digital/header_format_base.h>
#include <gnuradio/digital/header_buffer.h>
#include <gnuradio/logger.h>
#include <boost/enable_shared_from_this.hpp>

namespace gr {
  namespace digital {

    /*!
     * \brief Default header formatter for PDU formatting.
     * \ingroup packet_operators_blk
     *
     * \details
     * Used to handle the default packet header.
     *
     * See the parent class header_format_base for details of how
     * these classes operate.
     *
     * The default header created in this base class consists of an
     * access code and the packet length. The length is encoded as a
     * 16-bit value repeated twice:
     *
     * \verbatim
         | access code | hdr | payload |
       \endverbatim
     *
     * Where the access code is <= 64 bits and hdr is:
     *
     * \verbatim
         |  0 -- 15 | 16 -- 31 |
         | pkt len  | pkt len  |
       \endverbatim
     *
     * The access code and header are formatted for network byte order.
     *
     * This header generator does not calculate or append a CRC to the
     * packet. Use the CRC32 Async block for that before adding the
     * header. The header's length will then measure the payload plus
     * the CRC length (4 bytes for a CRC32).
     *
     * The default header parser produces a PMT dictionary that
     * contains the following keys. All formatter blocks MUST produce
     * these two values in any dictionary.
     *
     * \li "payload symbols": the number of symbols in the
     * payload. The payload decoder will have to know how this relates
     * to the number of bits received. This block knows nothing about
     * the payload modulation or the number of bits/symbol. Use the
     * gr::digital::header_format_counter for that purpose.
     *
     * \sa header_format_counter
     * \sa header_format_crc
     * \sa header_format_ofdm
     */
    class DIGITAL_API header_format_default
      : public header_format_base
    {
     public:
      header_format_default(const std::string &access_code, int threshold,
                            int bps);
      virtual ~header_format_default();

      /*!
       * Creates a header from the access code and packet length and
       * creates an output header as a PMT vector in the form:
       *
       * \verbatim
           | access code | pkt len | pkt len |
         \endverbatim
       *
       * \param nbytes_in The length (in bytes) of the \p input payload
       * \param input An array of unsigned chars of the packet payload
       * \param output A pmt::u8vector with the new header prepended
       *        onto the input data.
       * \param info A pmt::dict containing meta data and info about
       *        the PDU (generally from the metadata portion of the
       *        input PDU). Data can be extracted from this for the
       *        header formatting or inserted.
       */
      virtual bool format(int nbytes_in,
                          const unsigned char *input,
                          pmt::pmt_t &output,
                          pmt::pmt_t &info);

      /*!
       * Parses a header of the form:
       *
       * \verbatim
           | access code | pkt len | pkt len | payload |
         \endverbatim
       *
       * This is implemented as a state machine that starts off
       * searching for the access code. Once found, the access code is
       * used to find the start of the packet and the following
       * header. This default header encodes the length of the payload
       * a 16 bit integer twice. The state machine finds the header
       * and checks that both payload length values are the same. It
       * then goes into its final state that reads in the payload
       * (based on the payload length) and produces a payload as a PMT
       * u8 vector of packed bytes.
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
       */
      virtual bool parse(int nbits_in,
                         const unsigned char *input,
                         std::vector<pmt::pmt_t> &info,
                         int &nbits_processed);

      /*!
       * Returns the length of the formatted header in bits.
       */
      virtual size_t header_nbits() const;

      /*!
       * Updates the access code. Must be a string of 1's and 0's and
       * <= 64 bits.
       */
      bool set_access_code(const std::string &access_code);

      /*!
       * Returns the formatted access code as a 64-bit register.
       */
      unsigned long long access_code() const;

      /*!
       * Sets the threshold for number of access code bits can be in
       * error before detection. Defaults to 0.
       */
      void set_threshold(unsigned int thresh=0);

      /*!
       * Returns threshold value for access code detection.
       */
      unsigned int threshold() const;

      /*!
       * Factory to create an async packet header formatter; returns
       * an sptr to the object.
       *
       * \param access_code An access code that is used to find and
       * synchronize the start of a packet. Used in the parser and in
       * other blocks like a corr_est block that helps trigger the
       * receiver. Can be up to 64-bits long.
       * \param threshold How many bits can be wrong in the access
       * code and still count as correct.
       * \param bps The number of bits/second used in the payload's
       * modulator.
       */
      static sptr make(const std::string &access_code, int threshold,
                       int bps = 1);

    protected:
      uint64_t d_access_code;        //!< register to hold the access code
      size_t d_access_code_len;      //!< length in bits of the access code

      uint16_t d_bps;                //!< bits/sec of payload modulation

      unsigned long long d_data_reg; //!< used to look for access_code
      unsigned long long d_mask;     /*!< masks access_code bits (top N bits are set where
                                       N is the number of bits in the access code) */
      unsigned int d_threshold;      //!< how many bits may be wrong in sync vector

      int d_pkt_len;                 //!< Length of the packet to put into the output buffer
      int d_pkt_count;               //!< Number of bytes bits already received

      int d_nbits;                   //!< num bits processed since reset

      //! Access code found, start getting the header
      virtual void enter_have_sync();

      //! Header found, setup for pulling in the hard decision bits
      virtual void enter_have_header(int payload_len);

      //! Verify that the header is valid
      virtual bool header_ok();

      /*! Get info from the header; return payload length and package
       *  rest of data in d_info dictionary.
       */
      virtual int header_payload();
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HEADER_FORMAT_DEFAULT_H */
