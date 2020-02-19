/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef OR_BLK_H
#define OR_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief output = input_0 | input_1 | , ... | input_N)
 * \ingroup boolean_operators_blk
 *
 * Bitwise boolean or across all input streams.
 */
template <class T>
class BLOCKS_API or_blk : virtual public sync_block
{
public:
    // gr::blocks::or_blk::sptr
    typedef boost::shared_ptr<or_blk<T>> sptr;

    static sptr make(size_t vlen = 1);
};

typedef or_blk<short> or_ss;
typedef or_blk<int> or_ii;
typedef or_blk<char> or_bb;
} /* namespace blocks */
} /* namespace gr */

#endif /* OR_BLK_H */
