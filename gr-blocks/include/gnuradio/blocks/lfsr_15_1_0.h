/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GRI_LFSR_15_1_0_H
#define INCLUDED_GRI_LFSR_15_1_0_H

#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief Linear Feedback Shift Register using primitive polynomial x^15 + x + 1
 * \ingroup misc
 *
 * \details
 * Generates a maximal length pseudo-random sequence of length
 * 2^15 - 1 bits.
 */
class BLOCKS_API lfsr_15_1_0
{
private:
    unsigned long d_sr; // shift register

public:
    lfsr_15_1_0() { reset(); }

    void reset() { d_sr = 0x7fff; }

    int next_bit()
    {
        d_sr = ((((d_sr >> 1) ^ d_sr) & 0x1) << 14) | (d_sr >> 1);
        return d_sr & 0x1;
    }

    int next_byte()
    {
        int v = 0;
        for (int i = 0; i < 8; i++) {
            v >>= 1;
            if (next_bit())
                v |= 0x80;
        }
        return v;
    }
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GRI_LFSR_15_1_0_H */
