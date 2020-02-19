/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GRI_LFSR_32k_H
#define INCLUDED_GRI_LFSR_32k_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/blocks/lfsr_15_1_0.h>

namespace gr {
namespace blocks {

/*!
 * \brief generate pseudo-random sequence of length 32768 bits.
 * \ingroup misc
 *
 * \details
 * This is based on gri_lfsr_15_1_0 with an extra 0 added at the
 * end of the sequence.
 */
class BLOCKS_API lfsr_32k
{
private:
    lfsr_15_1_0 d_lfsr;
    unsigned int d_count;

public:
    lfsr_32k() { reset(); }

    void reset()
    {
        d_lfsr.reset();
        d_count = 0;
    }

    int next_bit()
    {
        if (d_count == 32767) {
            d_count = 0;
            return 0;
        }
        d_count++;
        return d_lfsr.next_bit();
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

    int next_short()
    {
        int v = 0;
        for (int i = 0; i < 16; i++) {
            v >>= 1;
            if (next_bit())
                v |= 0x8000;
        }
        return v;
    }
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GRI_LFSR_32k_H */
