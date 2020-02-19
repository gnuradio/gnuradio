/* -*- c++ -*- */
/* Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_HEADER_FORMAT_CRC_H
#define INCLUDED_DIGITAL_HEADER_FORMAT_CRC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/header_format_default.h>
#include <pmt/pmt.h>
#include <boost/crc.hpp>

namespace gr {
namespace digital {

/*!
 * \brief Header formatter that includes the payload length,
 * packet number, and a CRC check on the header.
 * \ingroup packet_operators_blk
 *
 * \details
 *
 * Child class of header_format_base. This version's header
 * format looks like:
 *
 * \li length (12 bits): length of the payload
 * \li number (12 bits): packet number
 * \li CRC8 (8 bits): A CRC8 check on the header contents
 *
 * Instead of duplicating the payload length, we only add it once
 * and use the CRC8 to make sure it's correctly received.
 *
 * \verbatim
     |  0 -- 11 | 12 -- 23 | 24 -- 31 |
     |    len   | pkt len  |   CRC8   |
   \endverbatim
 *
 * Reimplements packet_header_default in the style of the
 * header_format_base.
 */
class DIGITAL_API header_format_crc : public header_format_base
{
public:
    header_format_crc(const std::string& len_key_name = "packet_len",
                      const std::string& num_key_name = "packet_num");
    virtual ~header_format_crc();

    void set_header_num(unsigned header_num) { d_header_number = header_num; };

    /*!
     * \brief Encodes the header information in the given tags into
     * bits and places them into \p out.
     *
     * \details
     * Uses the following header format:
     *  - Bits 0-11: The packet length (what was stored in the tag with key \p
     * len_tag_key)
     *  - Bits 12-23: The header number (counts up everytime this function is called)
     *  - Bit 24-31: 8-Bit CRC
     */
    virtual bool format(int nbytes_in,
                        const unsigned char* input,
                        pmt::pmt_t& output,
                        pmt::pmt_t& info);

    virtual bool parse(int nbits_in,
                       const unsigned char* input,
                       std::vector<pmt::pmt_t>& info,
                       int& nbits_processed);

    /*!
     * Returns the length of the formatted header in bits.
     */
    virtual size_t header_nbits() const;

    /*!
     * Factory to create an async packet header formatter; returns
     * an sptr to the object.
     */
    static sptr make(const std::string& len_key_name = "packet_len",
                     const std::string& num_key_name = "packet_num");

protected:
    uint16_t d_header_number;
    pmt::pmt_t d_len_key_name;
    pmt::pmt_t d_num_key_name;
    boost::crc_optimal<8, 0x07, 0xFF, 0x00, false, false> d_crc_impl;

    //! Verify that the header is valid
    virtual bool header_ok();

    /*! Get info from the header; return payload length and package
     *  rest of data in d_info dictionary.
     */
    virtual int header_payload();
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HEADER_FORMAT_CRC_H */
