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

#include <gnuradio/blocks/unpack_k_bits.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace blocks {
namespace kernel {

unpack_k_bits::unpack_k_bits(unsigned k) : d_k(k)
{
    if (d_k == 0)
        throw std::out_of_range("unpack_k_bits: k must be > 0");
}

unpack_k_bits::~unpack_k_bits() {}

void unpack_k_bits::unpack(unsigned char* bits,
                           const unsigned char* bytes,
                           int nbytes) const
{
    int n = 0;
    for (int i = 0; i < nbytes; i++) {
        unsigned int t = bytes[i];
        for (int j = d_k - 1; j >= 0; j--)
            bits[n++] = (t >> j) & 0x01;
    }
}

void unpack_k_bits::unpack_rev(unsigned char* bits,
                               const unsigned char* bytes,
                               int nbytes) const
{
    int n = 0;
    for (int i = 0; i < nbytes; i++) {
        unsigned int t = bytes[i];
        for (unsigned int j = 0; j < d_k; j++)
            bits[n++] = (t >> j) & 0x01;
    }
}

int unpack_k_bits::k() const { return d_k; }

} /* namespace kernel */
} /* namespace blocks */
} /* namespace gr */
