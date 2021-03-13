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
#include <gnuradio/digital/header_format_counter.h>
#include <gnuradio/math.h>
#include <volk/volk_alloc.hh>
#include <cstring>
#include <iomanip>
#include <iostream>

namespace gr {
namespace digital {

header_format_counter::sptr
header_format_counter::make(const std::string& access_code, int threshold, int bps)
{
    return header_format_counter::sptr(
        new header_format_counter(access_code, threshold, bps));
}

header_format_counter::header_format_counter(const std::string& access_code,
                                             int threshold,
                                             int bps)
    : header_format_default(access_code, threshold, bps)
{
    d_counter = 0;
}

header_format_counter::~header_format_counter() {}

bool header_format_counter::format(int nbytes_in,
                                   const unsigned char* input,
                                   pmt::pmt_t& output,
                                   pmt::pmt_t& info)

{
    // Creating the output pmt copies data; free our own here when done.
    volk::vector<uint8_t> bytes_out(header_nbytes());

    header_buffer header(bytes_out.data());
    header.add_field64(d_access_code, d_access_code_len);
    header.add_field16((uint16_t)(nbytes_in));
    header.add_field16((uint16_t)(nbytes_in));
    header.add_field16((uint16_t)(d_bps));
    header.add_field16((uint16_t)(d_counter));

    // Package output data into a PMT vector
    output = pmt::init_u8vector(header_nbytes(), bytes_out.data());

    d_counter++;

    return true;
}

size_t header_format_counter::header_nbits() const
{
    return d_access_code_len + 8 * 4 * sizeof(uint16_t);
}

bool header_format_counter::header_ok()
{
    // confirm that two copies of header info are identical
    uint16_t len0 = d_hdr_reg.extract_field16(0);
    uint16_t len1 = d_hdr_reg.extract_field16(16);
    return (len0 ^ len1) == 0;
}

int header_format_counter::header_payload()
{
    uint16_t len = d_hdr_reg.extract_field16(0);
    uint16_t bps = d_hdr_reg.extract_field16(32);
    uint16_t counter = d_hdr_reg.extract_field16(48);

    d_bps = bps;

    d_info = pmt::make_dict();
    d_info = pmt::dict_add(
        d_info, pmt::intern("payload symbols"), pmt::from_long(8 * len / d_bps));
    d_info = pmt::dict_add(d_info, pmt::intern("bps"), pmt::from_long(bps));
    d_info = pmt::dict_add(d_info, pmt::intern("counter"), pmt::from_long(counter));
    return static_cast<int>(len);
}

} /* namespace digital */
} /* namespace gr */
