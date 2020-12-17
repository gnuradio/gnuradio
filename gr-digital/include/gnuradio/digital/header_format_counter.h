/* -*- c++ -*- */
/* Copyright 2015-2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_HEADER_FORMAT_COUNTER_H
#define INCLUDED_DIGITAL_HEADER_FORMAT_COUNTER_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/header_format_default.h>
#include <pmt/pmt.h>

namespace gr {
namespace digital {

/*!
 * \brief Header formatter that adds the payload bits/symbol
 * format and a packet number counter.
 * \ingroup packet_operators_blk
 *
 * \details
 *
 * Child class of header_format_default. This version adds two
 * fields to the header:
 *
 * \li bps (16 bits): bits/symbol used when modulating the payload.
 * \li count (16 bits): a counter for the packet number.
 *
 * Like the default packet formatter, the length is encoded as a
 * 16-bit value repeated twice. The full packet looks like:
 * \verbatim
     | access code | hdr | payload |
   \endverbatim
 *
 * Where the access code is <= 64 bits and hdr is:
 * \verbatim
     |  0 -- 15 | 16 -- 31 |
     | pkt len  | pkt len  |
     | bits/sym | counter  |
   \endverbatim
 *
 * The access code and header are formatted for network byte order.
 *
 * \sa header_format_default
 */
class DIGITAL_API header_format_counter : public header_format_default
{
public:
    typedef std::shared_ptr<header_format_counter> sptr;

    header_format_counter(const std::string& access_code, int threshold, int bps);
    ~header_format_counter() override;

    /*!
     * Creates a header from the access code and packet length to
     * build an output packet in the form:
     *
     * \verbatim
         | access code | pkt len | pkt len | bps | counter |
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
    bool format(int nbytes_in,
                const unsigned char* input,
                pmt::pmt_t& output,
                pmt::pmt_t& info) override;

    /*!
     * Returns the length of the formatted header in bits.
     */
    size_t header_nbits() const override;

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
    static sptr make(const std::string& access_code, int threshold, int bps);

protected:
    uint16_t d_counter; //!< keeps track of the number of packets transmitted

    //! Verify that the header is valid
    bool header_ok() override;

    /*! Get info from the header; return payload length and package
     *  rest of data in d_info dictionary.
     *
     * Extracts the header of the form:
     *
     * \verbatim
         | access code | pkt len | pkt len | bps | counter | payload |
       \endverbatim
     */
    int header_payload() override;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HEADER_FORMAT_COUNTER_H */
