/* -*- c++ -*- */
/* Copyright 2015 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/digital/header_buffer.h>
#include <string.h>
#include <volk/volk.h>
#include <algorithm>
#include <stdexcept>

namespace gr {
namespace digital {

header_buffer::header_buffer(uint8_t* bytes_out)
{
    d_offset = 0;
    d_buffer = bytes_out;
}

header_buffer::~header_buffer() {}

void header_buffer::clear()
{
    if (d_buffer) // TX mode
        d_offset = 0;
    else // RX mode
        d_input.clear();
}

size_t header_buffer::length() const
{
    if (d_buffer) // TX mode
        return d_offset;
    else // RX mode
        return d_input.size();
}

const uint8_t* header_buffer::header() const { return d_buffer; }

void header_buffer::add_field8(uint8_t data, int len, bool bs)
{
    int nbytes = len / 8;
    if (d_buffer) {
        memcpy(&d_buffer[d_offset], &data, nbytes);
        d_offset += nbytes;
    }
}

void header_buffer::add_field16(uint16_t data, int len, bool bs)
{
    int nbytes = len / 8;
    if (d_buffer) {
        uint16_t x = data;
        if (!bs) {
            volk_16u_byteswap(&x, 1);
            x = x >> (16 - len);
        }
        memcpy(&d_buffer[d_offset], &x, nbytes);
        d_offset += nbytes;
    }
}

void header_buffer::add_field32(uint32_t data, int len, bool bs)
{
    int nbytes = len / 8;
    if (d_buffer) {
        uint32_t x = data;
        if (!bs) {
            volk_32u_byteswap(&x, 1);
            x = x >> (32 - len);
        }
        memcpy(&d_buffer[d_offset], &x, nbytes);
        d_offset += nbytes;
    }
}

void header_buffer::add_field64(uint64_t data, int len, bool bs)
{
    int nbytes = len / 8;
    if (d_buffer) {
        uint64_t x = data;
        if (!bs) {
            volk_64u_byteswap(&x, 1);
            x = x >> (64 - len);
        }
        memcpy(&d_buffer[d_offset], &x, nbytes);
        d_offset += nbytes;
    }
}

void header_buffer::insert_bit(int bit) { d_input.push_back(bit); }

uint8_t header_buffer::extract_field8(int pos, int len, bool bs)
{
    if (len > 8) {
        throw std::runtime_error("header_buffer::extract_field for "
                                 "uint8_t length must be <= 8");
    }

    uint8_t field = 0x00;
    std::vector<bool>::iterator itr;
    for (itr = d_input.begin() + pos; itr != d_input.begin() + pos + len; itr++) {
        field = (field << 1) | ((*itr) & 0x1);
    }

    return field;
}

uint16_t header_buffer::extract_field16(int pos, int len, bool bs)
{
    if (len > 16) {
        throw std::runtime_error("header_buffer::extract_field for "
                                 "uint16_t length must be <= 16");
    }

    uint16_t field = 0x0000;
    std::vector<bool>::iterator itr;
    for (itr = d_input.begin() + pos; itr != d_input.begin() + pos + len; itr++) {
        field = (field << 1) | ((*itr) & 0x1);
    }

    if (bs) {
        volk_16u_byteswap(&field, 1);
    }

    return field;
}

uint32_t header_buffer::extract_field32(int pos, int len, bool bs)
{
    if (len > 32) {
        throw std::runtime_error("header_buffer::extract_field for "
                                 "uint32_t length must be <= 32");
    }

    uint32_t field = 0x00000000;
    std::vector<bool>::iterator itr;
    for (itr = d_input.begin() + pos; itr != d_input.begin() + pos + len; itr++) {
        field = (field << 1) | ((*itr) & 0x1);
    }

    if (bs) {
        volk_32u_byteswap(&field, 1);
    }

    return field;
}

uint64_t header_buffer::extract_field64(int pos, int len, bool bs)
{
    if (len > 64) {
        throw std::runtime_error("header_buffer::extract_field for "
                                 "uint64_t length must be <= 64");
    }

    uint64_t field = 0x0000000000000000;
    std::vector<bool>::iterator itr;
    for (itr = d_input.begin() + pos; itr != d_input.begin() + pos + len; itr++) {
        field = (field << 1) | ((*itr) & 0x1);
    }

    if (bs) {
        volk_64u_byteswap(&field, 1);
    }

    return field;
}

} /* namespace digital */
} /* namespace gr */
