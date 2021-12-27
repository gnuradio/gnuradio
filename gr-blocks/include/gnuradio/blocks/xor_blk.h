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
    using sptr = std::shared_ptr<xor_blk<T>>;

    static sptr make(size_t vlen = 1);
};

using xor_bb = xor_blk<std::uint8_t>;
using xor_ss = xor_blk<std::int16_t>;
using xor_ii = xor_blk<std::int32_t>;
} /* namespace blocks */
} /* namespace gr */

#endif /* XOR_BLK_H */
