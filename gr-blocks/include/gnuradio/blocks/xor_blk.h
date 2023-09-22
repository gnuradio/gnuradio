/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef XOR_BLK_H
#define XOR_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = input_0 ^ input_1 ^ , ... ^ input_N)
 * \ingroup boolean_operators_blk
 *
 * Bitwise boolean xor across all input streams.
 */
template <class T>
class BLOCKS_API xor_blk : virtual public sync_block
{
public:
    // gr::blocks::xor::sptr
    typedef std::shared_ptr<xor_blk<T>> sptr;

    static sptr make(size_t vlen = 1);
};

typedef xor_blk<std::uint8_t> xor_bb;
typedef xor_blk<std::int16_t> xor_ss;
typedef xor_blk<std::int32_t> xor_ii;
} /* namespace blocks */
} /* namespace gr */

#endif /* XOR_BLK_H */
