/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef _GR_COUNT_BITS_H_
#define _GR_COUNT_BITS_H_

#include <gnuradio/blocks/api.h>

#include <cstdint>

namespace gr {
namespace blocks {

//! return number of set bits in the low  8 bits of x
BLOCKS_API unsigned int count_bits8(unsigned int x);

//! return number of set bits in the low 16 bits of x
BLOCKS_API unsigned int count_bits16(unsigned int x);

//! return number of set bits in the low 32 bits of x
BLOCKS_API unsigned int count_bits32(unsigned int x);

//! return number of set bits in a 64-bit word
BLOCKS_API unsigned int count_bits64(uint64_t x);

} /* namespace blocks */
} /* namespace gr */

#endif /* _GR_COUNT_BITS_H_ */
