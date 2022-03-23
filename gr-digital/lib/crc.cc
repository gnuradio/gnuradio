/* -*- c++ -*- */
/*
 * Copyright 2022 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/digital/crc.h>
#include <stdexcept>

namespace gr {
namespace digital {

crc::crc(unsigned num_bits,
         uint64_t poly,
         uint64_t initial_value,
         uint64_t final_xor,
         bool input_reflected,
         bool result_reflected)
    : d_num_bits(num_bits),
      d_mask(num_bits == 64 ? ~static_cast<uint64_t>(0)
                            : (static_cast<uint64_t>(1) << num_bits) - 1),
      d_initial_value(initial_value & d_mask),
      d_final_xor(final_xor & d_mask),
      d_input_reflected(input_reflected),
      d_result_reflected(result_reflected)
{
    if ((num_bits < 8) || (num_bits > 64)) {
        throw std::runtime_error("CRC number of bits must be between 8 and 64");
    }

    d_table[0] = 0;
    if (d_input_reflected) {
        poly = reflect(poly);
        uint64_t crc = 1;
        int i = 128;
        do {
            if (crc & 1) {
                crc = (crc >> 1) ^ poly;
            } else {
                crc >>= 1;
            }
            for (int j = 0; j < 256; j += 2 * i) {
                d_table[i + j] = (crc ^ d_table[j]) & d_mask;
            }
            i >>= 1;
        } while (i > 0);
    } else {
        const uint64_t msb = static_cast<uint64_t>(1) << (num_bits - 1);
        uint64_t crc = msb;
        int i = 1;
        do {
            if (crc & msb) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
            for (int j = 0; j < i; ++j) {
                d_table[i + j] = (crc ^ d_table[j]) & d_mask;
            }
            i <<= 1;
        } while (i < 256);
    }
}

crc::~crc() {}

uint64_t crc::compute(const uint8_t* data, std::size_t len)
{
    uint64_t rem = d_initial_value;

    if (d_input_reflected) {
        for (std::size_t i = 0; i < len; ++i) {
            uint8_t byte = data[i];
            uint8_t idx = (rem ^ byte) & 0xff;
            rem = d_table[idx] ^ (rem >> 8);
        }
    } else {
        for (std::size_t i = 0; i < len; ++i) {
            uint8_t byte = data[i];
            uint8_t idx = ((rem >> (d_num_bits - 8)) ^ byte) & 0xff;
            rem = (d_table[idx] ^ (rem << 8)) & d_mask;
        }
    }

    if (d_input_reflected != d_result_reflected) {
        rem = reflect(rem);
    }

    rem = rem ^ d_final_xor;
    return rem;
}

uint64_t crc::reflect(uint64_t word) const
{
    uint64_t ret;
    ret = word & 1;
    for (unsigned i = 1; i < d_num_bits; ++i) {
        word >>= 1;
        ret = (ret << 1) | (word & 1);
    }
    return ret;
}

} /* namespace digital */
} /* namespace gr */
