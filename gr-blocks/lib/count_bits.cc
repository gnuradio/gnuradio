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

/*
 * these are slow and obvious.  If you need something faster, fix these
 *
 * Can probably replace with VOLK's popcount
 */

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
    unsigned res = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
    res = (res & 0x0F0F0F0F) + ((res >> 4) & 0x0F0F0F0F);
    res = (res & 0x00FF00FF) + ((res >> 8) & 0x00FF00FF);
    return (res & 0x0000FFFF) + ((res >> 16) & 0x0000FFFF);
}

unsigned int count_bits64(unsigned long long x)
{
    return count_bits32((x >> 32) & 0xffffffff) + count_bits32(x & 0xffffffff);
}

} /* namespace blocks */
} /* namespace gr */
