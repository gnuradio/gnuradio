/* -*- c++ -*- */
/* Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/digital/header_buffer.h>
#include <gnuradio/digital/header_format_crc.h>
#include <string.h>
#include <volk/volk_alloc.hh>

namespace gr {
namespace digital {

header_format_crc::sptr header_format_crc::make(const std::string& len_key_name,
                                                const std::string& num_key_name)
{
    return header_format_crc::sptr(new header_format_crc(len_key_name, num_key_name));
}

header_format_crc::header_format_crc(const std::string& len_key_name,
                                     const std::string& num_key_name)
    : header_format_base(), d_header_number(0)
{
    d_len_key_name = pmt::intern(len_key_name);
    d_num_key_name = pmt::intern(num_key_name);
}

header_format_crc::~header_format_crc() {}

bool header_format_crc::format(int nbytes_in,
                               const unsigned char* input,
                               pmt::pmt_t& output,
                               pmt::pmt_t& info)
{
    // Creating the output pmt copies data; free our own here when done.
    volk::vector<uint8_t> bytes_out(header_nbytes());

    // Should this throw instead of mask if the payload is too big
    // for 12-bit representation?
    nbytes_in &= 0x0FFF;

    d_crc_impl.reset();
    d_crc_impl.process_bytes((void const*)&nbytes_in, 2);
    d_crc_impl.process_bytes((void const*)&d_header_number, 2);
    uint8_t crc = d_crc_impl();

    // Form 2 12-bit items into 1 2-byte item
    uint32_t concat = 0;
    concat = (d_header_number << 12) | (nbytes_in);

    header_buffer header(bytes_out.data());
    header.add_field32(concat, 24, true);
    header.add_field8(crc);

    d_header_number++;
    d_header_number &= 0x0FFF;

    // Package output data into a PMT vector
    output = pmt::init_u8vector(header_nbytes(), bytes_out.data());

    return true;
}

bool header_format_crc::parse(int nbits_in,
                              const unsigned char* input,
                              std::vector<pmt::pmt_t>& info,
                              int& nbits_processed)
{
    while (nbits_processed <= nbits_in) {
        d_hdr_reg.insert_bit(input[nbits_processed++]);
        if (d_hdr_reg.length() == header_nbits()) {
            if (header_ok()) {
                int payload_len = header_payload();
                enter_have_header(payload_len);
                info.push_back(d_info);
                d_hdr_reg.clear();
                return true;
            } else {
                d_hdr_reg.clear();
                return false;
            }
            break;
        }
    }

    return true;
}

size_t header_format_crc::header_nbits() const { return 32; }


bool header_format_crc::header_ok()
{
    uint32_t pkt = d_hdr_reg.extract_field32(0, 24, true);
    uint16_t pktlen = static_cast<uint16_t>((pkt >> 8) & 0x0fff);
    uint16_t pktnum = static_cast<uint16_t>((pkt >> 20) & 0x0fff);
    uint8_t crc_rcvd = d_hdr_reg.extract_field8(24);

    // Check CRC8
    d_crc_impl.reset();
    d_crc_impl.process_bytes((void const*)&pktlen, 2);
    d_crc_impl.process_bytes((void const*)&pktnum, 2);
    uint8_t crc_clcd = d_crc_impl();

    return (crc_rcvd == crc_clcd);
}

int header_format_crc::header_payload()
{
    uint32_t pkt = d_hdr_reg.extract_field32(0, 24, true);
    uint16_t pktlen = static_cast<uint16_t>((pkt >> 8) & 0x0fff);
    uint16_t pktnum = static_cast<uint16_t>((pkt >> 20) & 0x0fff);

    d_info = pmt::make_dict();
    d_info = pmt::dict_add(d_info, d_len_key_name, pmt::from_long(8 * pktlen));
    d_info = pmt::dict_add(d_info, d_num_key_name, pmt::from_long(pktnum));
    return static_cast<int>(pktlen);
}

} /* namespace digital */
} /* namespace gr */
