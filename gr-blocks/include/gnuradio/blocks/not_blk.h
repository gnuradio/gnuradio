/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
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
    typedef boost::shared_ptr<not_blk<T>> sptr;

    static sptr make(size_t vlen = 1);
};

typedef not_blk<std::uint8_t> not_bb;
typedef not_blk<std::int16_t> not_ss;
typedef not_blk<std::int32_t> not_ii;
} /* namespace blocks */
} /* namespace gr */

#endif /* NOT_BLK_H */
