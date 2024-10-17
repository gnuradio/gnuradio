/* -*- c++ -*- */
/*
 * Copyright 2007,2012,2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/digital/glfsr.h>
#include <stdexcept>

namespace gr {
namespace digital {

static uint64_t s_polynomial_masks[] = {
    0x0000000000000000, // Placeholder for 1-indexing
    0x0000000000000001, // x^1 + 1
    0x0000000000000003, // x^2 + x^1 + 1
    0x0000000000000006, // x^3 + x^2 + 1
    0x000000000000000C, // x^4 + x^3 + 1
    0x0000000000000014, // x^5 + x^3 + 1
    0x0000000000000030, // x^6 + x^5 + 1
    0x0000000000000060, // x^7 + x^6 + 1
    0x00000000000000B8, // x^8 + x^6 + x^5 + x^4 + 1
    0x0000000000000110, // x^9 + x^5 + 1
    0x0000000000000240, // x^10 + x^7 + 1
    0x0000000000000500, // x^11 + x^9 + 1
    0x0000000000000829, // x^12 + x^6 + x^4 + x^1 + 1
    0x000000000000100D, // x^13 + x^4 + x^3 + x^1 + 1
    0x0000000000002015, // x^14 + x^5 + x^3 + x^1 + 1
    0x0000000000006000, // x^15 + x^14 + 1
    0x000000000000D008, // x^16 + x^15 + x^13 + x^4 + 1
    0x0000000000012000, // x^17 + x^14 + 1
    0x0000000000020400, // x^18 + x^11 + 1
    0x0000000000040023, // x^19 + x^6 + x^2 + x^1 + 1
    0x0000000000090000, // x^20 + x^17 + 1
    0x0000000000140000, // x^21 + x^19 + 1
    0x0000000000300000, // x^22 + x^21 + 1
    0x0000000000420000, // x^23 + x^18 + 1
    0x0000000000E10000, // x^24 + x^23 + x^22 + x^17 + 1
    0x0000000001200000, // x^25 + x^22 + 1
    0x0000000002000023, // x^26 + x^6 + x^2 + x^1 + 1
    0x0000000004000013, // x^27 + x^5 + x^2 + x^1 + 1
    0x0000000009000000, // x^28 + x^25 + 1
    0x0000000014000000, // x^29 + x^27 + 1
    0x0000000020000029, // x^30 + x^6 + x^4 + x^1 + 1
    0x0000000048000000, // x^31 + x^28 + 1
    0x0000000080200003, // x^32 + x^22 + x^2 + x^1 + 1
    0x0000000100080000, // x^33 + x^20 + 1
    0x0000000204000003, // x^34 + x^27 + x^2 + x^1 + 1
    0x0000000500000000, // x^35 + x^33 + 1
    0x0000000801000000, // x^36 + x^25 + 1
    0x000000100000001F, // x^37 + x^5 + x^4 + x^3 + x^2 + x^1 + 1
    0x0000002000000031, // x^38 + x^6 + x^5 + x^1 + 1
    0x0000004400000000, // x^39 + x^35 + 1
    0x000000A000140000, // x^40 + x^38 + x^21 + x^19 + 1
    0x0000012000000000, // x^41 + x^38 + 1
    0x00000300000C0000, // x^42 + x^41 + x^20 + x^19 + 1
    0x0000063000000000, // x^43 + x^42 + x^38 + x^37 + 1
    0x00000C0000030000, // x^44 + x^43 + x^18 + x^17 + 1
    0x00001B0000000000, // x^45 + x^44 + x^42 + x^41 + 1
    0x0000300003000000, // x^46 + x^45 + x^26 + x^25 + 1
    0x0000420000000000, // x^47 + x^42 + 1
    0x0000C00000180000, // x^48 + x^47 + x^21 + x^20 + 1
    0x0001008000000000, // x^49 + x^40 + 1
    0x0003000000C00000, // x^50 + x^49 + x^24 + x^23 + 1
    0x0006000C00000000, // x^51 + x^50 + x^36 + x^35 + 1
    0x0009000000000000, // x^52 + x^49 + 1
    0x0018003000000000, // x^53 + x^52 + x^38 + x^37 + 1
    0x0030000000030000, // x^54 + x^53 + x^18 + x^17 + 1
    0x0040000040000000, // x^55 + x^31 + 1
    0x00C0000600000000, // x^56 + x^55 + x^35 + x^34 + 1
    0x0102000000000000, // x^57 + x^50 + 1
    0x0200004000000000, // x^58 + x^39 + 1
    0x0600003000000000, // x^59 + x^58 + x^38 + x^37 + 1
    0x0C00000000000000, // x^60 + x^59 + 1
    0x1800300000000000, // x^61 + x^60 + x^46 + x^45 + 1
    0x3000000000000030, // x^62 + x^61 + x^6 + x^5 + 1
    0x6000000000000000, // x^63 + x^62 + 1
    0xD800000000000000, // x^64 + x^63 + x^61 + x^60 + 1
};

glfsr::~glfsr() {}

uint64_t glfsr::glfsr_mask(unsigned int degree)
{
    if (degree < 1 || degree > 64)
        throw std::runtime_error("glfsr::glfsr_mask(): must be 1 <= degree <= 64");
    return s_polynomial_masks[degree];
}

uint8_t glfsr::next_bit()
{
    unsigned char bit = d_shift_register & 0x1;
    d_shift_register >>= 1;
    if (bit)
        d_shift_register ^= d_mask;
    return bit;
}

} /* namespace digital */
} /* namespace gr */
