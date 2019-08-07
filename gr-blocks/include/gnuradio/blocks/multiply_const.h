/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
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


#ifndef MULTIPLY_CONST_H
#define MULTIPLY_CONST_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = input * constant
 * \ingroup math_operators_blk
 */
template <class T>
class BLOCKS_API multiply_const : virtual public sync_block
{

public:
    // gr::blocks::multiply_const::sptr
    typedef boost::shared_ptr<multiply_const<T>> sptr;

    /*!
     * \brief Create an instance of multiply_const
     * \param k multiplicative constant
     * \param vlen number of items in vector
     */
    static sptr make(T k, size_t vlen = 1);

    /*!
     * \brief Return multiplicative constant
     */
    virtual T k() const = 0;

    /*!
     * \brief Set multiplicative constant
     */
    virtual void set_k(T k) = 0;
};

typedef multiply_const<std::int16_t> multiply_const_ss;
typedef multiply_const<std::int32_t> multiply_const_ii;
typedef multiply_const<float> multiply_const_ff;
typedef multiply_const<gr_complex> multiply_const_cc;
} /* namespace blocks */
} /* namespace gr */

#endif /* MULTIPLY_CONST_H */
