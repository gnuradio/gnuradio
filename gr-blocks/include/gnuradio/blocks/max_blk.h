/* -*- c++ -*- */
/*
 * Copyright 2014,2015,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef MAX_BLK_H
#define MAX_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Compares vectors from multiple streams and determines
 * the maximum value from each vector over all streams.
 * \ingroup math_operators_blk
 *
 * \details
 * Data is passed in as a vector of length \p vlen from multiple
 * input sources.  If vlen_out == 1 then
 * It will look through these streams of \p vlen
 * data items and the output stream will contain the maximum value
 * in the vector.
 * If vlen_out == vlen and not equal to 1 then
 * output will be a vector with individual items selected from
 * the maximum corresponding input vector items.
 */
template <class T>
class BLOCKS_API max_blk : virtual public sync_block
{
public:
    // gr::blocks::max_blk::sptr
    typedef boost::shared_ptr<max_blk<T>> sptr;

    static sptr make(size_t vlen, size_t vlen_out = 1);
};

typedef max_blk<std::int16_t> max_ss;
typedef max_blk<std::int32_t> max_ii;
typedef max_blk<float> max_ff;
} /* namespace blocks */
} /* namespace gr */

#endif /* MAX_BLK_H */
