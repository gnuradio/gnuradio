/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef NOT_BLK_H
#define NOT_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = ~input
 * \ingroup boolean_operators_blk
 *
 * bitwise boolean not of input streams.
 */
template <class T>
class BLOCKS_API not_blk : virtual public sync_block
{
public:
    using sptr = std::shared_ptr<not_blk<T>>;

    static sptr make(size_t vlen = 1);
};

using not_bb = not_blk<std::uint8_t>;
using not_ss = not_blk<std::int16_t>;
using not_ii = not_blk<std::int32_t>;
} /* namespace blocks */
} /* namespace gr */

#endif /* NOT_BLK_H */
