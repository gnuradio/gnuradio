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
    0x00000000,
    0x00000001, // x^1 + 1
    0x00000003, // x^2 + x^1 + 1
    0x00000005, // x^3 + x^1 + 1
    0x00000009, // x^4 + x^1 + 1
    0x00000012, // x^5 + x^2 + 1
    0x00000021, // x^6 + x^1 + 1
    0x00000041, // x^7 + x^1 + 1
    0x0000008E, // x^8 + x^4 + x^3 + x^2 + 1
    0x00000108, // x^9 + x^4 + 1
    0x00000204, // x^10 + x^4 + 1
    0x00000402, // x^11 + x^2 + 1
    0x00000829, // x^12 + x^6 + x^4 + x^1 + 1
    0x0000100D, // x^13 + x^4 + x^3 + x^1 + 1
    0x00002015, // x^14 + x^5 + x^3 + x^1 + 1
    0x00004001, // x^15 + x^1 + 1
    0x00008016, // x^16 + x^5 + x^3 + x^2 + 1
    0x00010004, // x^17 + x^3 + 1
    0x00020013, // x^18 + x^5 + x^2 + x^1 + 1
    0x00040013, // x^19 + x^5 + x^2 + x^1 + 1
    0x00080004, // x^20 + x^3 + 1
    0x00100002, // x^21 + x^2 + 1
    0x00200001, // x^22 + x^1 + 1
    0x00400010, // x^23 + x^5 + 1
    0x0080000D, // x^24 + x^4 + x^3 + x^1 + 1
    0x01000004, // x^25 + x^3 + 1
    0x02000023, // x^26 + x^6 + x^2 + x^1 + 1
    0x04000013, // x^27 + x^5 + x^2 + x^1 + 1
    0x08000004, // x^28 + x^3 + 1
    0x10000002, // x^29 + x^2 + 1
    0x20000029, // x^30 + x^4 + x^1 + 1
    0x40000004, // x^31 + x^3 + 1
    0x80000057  // x^32 + x^7 + x^5 + x^3 + x^2 + x^1 + 1
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
