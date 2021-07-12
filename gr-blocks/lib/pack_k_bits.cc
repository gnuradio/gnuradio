/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/blocks/pack_k_bits.h>
#include <stdexcept>

namespace gr {
namespace blocks {
namespace kernel {

pack_k_bits::pack_k_bits(unsigned k) : d_k(k)
{
    if (d_k == 0)
        throw std::out_of_range("pack_k_bits: k must be > 0");
}

pack_k_bits::~pack_k_bits() {}

void pack_k_bits::pack(unsigned char* bytes, const unsigned char* bits, int nbytes) const
{
    for (int i = 0; i < nbytes; i++) {
        bytes[i] = 0x00;
        for (unsigned int j = 0; j < d_k; j++) {
            bytes[i] |= (0x01 & bits[i * d_k + j]) << (d_k - j - 1);
        }
    }
}

void pack_k_bits::pack_rev(unsigned char* bytes,
                           const unsigned char* bits,
                           int nbytes) const
{
    for (int i = 0; i < nbytes; i++) {
        bytes[i] = 0x00;
        for (unsigned int j = 0; j < d_k; j++) {
            bytes[i] |= (0x01 & bits[i * d_k + j]) << j;
        }
    }
}

} /* namespace kernel */
} /* namespace blocks */
} /* namespace gr */
