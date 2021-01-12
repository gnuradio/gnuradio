/* -*- c++ -*- */
/*
 * Copyright 2014,2015,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef MIN_BLK_H
#define MIN_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Compares vectors from multiple streams and determines
 * the minimum value from each vector over all streams.
 * \ingroup math_operators_blk
 *
 * \details
 * Data is passed in as a vector of length \p vlen from multiple
 * input sources.  If vlen_out == 1 then
 * It will look through these streams of \p vlen
 * data items and the output stream will contain the minimum value
 * in the vector.
 * If vlen_out == vlen and not equal to 1 then
 * output will be a vector with individual items selected from
 * the minimum corresponding input vector items.
 */
template <class T>
class BLOCKS_API min_blk : virtual public sync_block
{
public:
    typedef std::shared_ptr<min_blk<T>> sptr;

    static sptr make(size_t vlen, size_t vlen_out = 1);
};

typedef min_blk<std::int16_t> min_ss;
typedef min_blk<std::int32_t> min_ii;
typedef min_blk<float> min_ff;
} /* namespace blocks */
} /* namespace gr */

#endif /* MIN_BLK_H */
