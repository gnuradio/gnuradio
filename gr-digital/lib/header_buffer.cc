/* -*- c++ -*- */
/* Copyright 2015 Free Software Foundation, Inc.
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
#include <volk/volk.h>
#include <algorithm>
#include <cstring>
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
    add_field64(data, len, bs);
}

void header_buffer::add_field32(uint32_t data, int len, bool bs)
{
    add_field64(data, len, bs);
}

void header_buffer::add_field64(uint64_t data, int len, bool bs)
{
    int nbytes = len / 8;
    if (d_buffer) {
        for (int i = 0; i < nbytes; i++) {
            int shift = bs ? i : (nbytes - i - 1);
            d_buffer[d_offset++] = (data >> (8 * shift)) & 0xff;
        }
    }
}

void header_buffer::insert_bit(int bit) { d_input.push_back(bit); }

template <class T>
T header_buffer::extract_field(int pos, int len, bool bs, bool lsb_first)
{
    if (len > 8 * (int)sizeof(T)) {
        throw std::runtime_error(
            std::string("header_buffer::extract_field for length must be <= ") +
            std::to_string(8 * sizeof(T)));
    }

    T field = 0;
    std::vector<bool>::iterator itr;
    if (lsb_first) {
        for (itr = d_input.begin() + pos + len - 1; itr >= d_input.begin() + pos; itr--) {
            field = (field << 1) | ((*itr) & 0x1);
        }
    } else {
        for (itr = d_input.begin() + pos; itr != d_input.begin() + pos + len; itr++) {
            field = (field << 1) | ((*itr) & 0x1);
        }
    }

    return field;
}

template DIGITAL_API uint8_t header_buffer::extract_field(int, int, bool, bool);
template DIGITAL_API uint16_t header_buffer::extract_field(int, int, bool, bool);
template DIGITAL_API uint32_t header_buffer::extract_field(int, int, bool, bool);
template DIGITAL_API uint64_t header_buffer::extract_field(int, int, bool, bool);

} /* namespace digital */
} /* namespace gr */
