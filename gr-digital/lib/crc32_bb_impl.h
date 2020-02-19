/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CRC32_BB_IMPL_H
#define INCLUDED_DIGITAL_CRC32_BB_IMPL_H

#include <gnuradio/digital/crc32_bb.h>
#include <boost/crc.hpp>

namespace gr {
namespace digital {

class crc32_bb_impl : public crc32_bb
{
private:
    bool d_check;
    bool d_packed;
    boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true> d_crc_impl;
    unsigned int d_crc_length;
    std::vector<char> d_buffer;
    unsigned int calculate_crc32(const unsigned char* in, size_t packet_length);

public:
    crc32_bb_impl(bool check, const std::string& lengthtagname, bool packed);
    ~crc32_bb_impl();

    int calculate_output_stream_length(const gr_vector_int& ninput_items);
    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CRC32_BB_IMPL_H */
