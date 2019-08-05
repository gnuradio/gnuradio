/* -*- c++ -*- */
/*
 * Copyright 2004, 2009, 2012, 2018 Free Software Foundation, Inc.
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


#ifndef MULTIPLY_H
#define MULTIPLY_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = prod (input_0, input_1, ...)
 * \ingroup math_operators_blk
 *
 * \details
 * Multiply across all input streams.
 */
template <class T>
class BLOCKS_API multiply : virtual public sync_block
{
public:
    // gr::blocks::multiply::sptr
    typedef boost::shared_ptr<multiply<T>> sptr;

    static sptr make(size_t vlen = 1);
};

typedef multiply<std::int16_t> multiply_ss;
typedef multiply<std::int32_t> multiply_ii;
typedef multiply<float> multiply_ff;
typedef multiply<gr_complex> multiply_cc;
} /* namespace blocks */
} /* namespace gr */

#endif /* MULTIPLY_H */
