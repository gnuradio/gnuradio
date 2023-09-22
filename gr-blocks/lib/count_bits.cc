/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/blocks/count_bits.h>

#include <volk/volk.h>

namespace gr {
namespace blocks {

unsigned int count_bits8(unsigned int x)
{
    int count = 0;

    for (int i = 0; i < 8; i++) {
        if (x & (1 << i))
            count++;
    }

    return count;
}

unsigned int count_bits16(unsigned int x)
{
    int count = 0;

    for (int i = 0; i < 16; i++) {
        if (x & (1 << i))
            count++;
    }

    return count;
}

unsigned int count_bits32(unsigned int x)
{
    unsigned res = 0;
    volk_32u_popcnt(&res, x);

    return res;
}

unsigned int count_bits64(uint64_t x)
{
    uint64_t res_as_u64 = 0;
    volk_64u_popcnt(&res_as_u64, x);

    return static_cast<unsigned int>(res_as_u64);
}

} /* namespace blocks */
} /* namespace gr */
