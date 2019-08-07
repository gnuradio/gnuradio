/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
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


#ifndef DIVIDE_H
#define DIVIDE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = input[0] / input[1] / ... / input[M-1]
 * \ingroup math_operators_blk
 *
 * \details
 * Divide across all input streams.
 */
template <class T>
class BLOCKS_API divide : virtual public sync_block
{
public:
    // gr::blocks::divide::sptr
    typedef boost::shared_ptr<divide<T>> sptr;

    static sptr make(size_t vlen = 1);
};

typedef divide<std::int16_t> divide_ss;
typedef divide<std::int32_t> divide_ii;
typedef divide<float> divide_ff;
typedef divide<gr_complex> divide_cc;
} /* namespace blocks */
} /* namespace gr */

#endif /* DIVIDE_H */
