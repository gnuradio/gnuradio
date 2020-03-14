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
    typedef std::shared_ptr<not_blk<T>> sptr;

    static sptr make(size_t vlen = 1);
};

typedef not_blk<std::uint8_t> not_bb;
typedef not_blk<std::int16_t> not_ss;
typedef not_blk<std::int32_t> not_ii;
} /* namespace blocks */
} /* namespace gr */

#endif /* NOT_BLK_H */
