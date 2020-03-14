/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef AND_BLK_H
#define AND_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = input[0] & input[1] & ... & input[M-1]
 * \ingroup boolean_operators_blk
 *
 * \details
 * bitwise boolean AND across all input streams.
 */
template <class T>
class BLOCKS_API and_blk : virtual public sync_block
{
public:
    // gr::blocks::and_blk::sptr
    typedef std::shared_ptr<and_blk<T>> sptr;

    static sptr make(size_t vlen = 1);
};

typedef and_blk<std::uint8_t> and_bb;
typedef and_blk<std::int16_t> and_ss;
typedef and_blk<std::int32_t> and_ii;
} /* namespace blocks */
} /* namespace gr */

#endif /* AND_BLK_H */
