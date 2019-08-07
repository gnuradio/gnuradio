/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010,2012,2018 Free Software Foundation, Inc.
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

#ifndef ADD_CONST_V_H
#define ADD_CONST_V_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output[m] = input[m] + constant vector for all M streams.
 * \ingroup math_operators_blk
 */
template <class T>
class BLOCKS_API add_const_v : virtual public sync_block
{

public:
    typedef boost::shared_ptr<add_const_v<T>> sptr;

    /*!
     * \brief Create an instance of add_const_v
     * \param k additive constant vector
     */
    static sptr make(std::vector<T> k);

    /*!
     * \brief Return additive constant vector
     */
    virtual std::vector<T> k() const = 0;

    /*!
     * \brief Set additive constant vector
     */
    virtual void set_k(std::vector<T> k) = 0;
};

typedef add_const_v<std::uint8_t> add_const_vbb;
typedef add_const_v<std::int16_t> add_const_vss;
typedef add_const_v<std::int32_t> add_const_vii;
typedef add_const_v<float> add_const_vff;
typedef add_const_v<gr_complex> add_const_vcc;

} /* namespace blocks */
} /* namespace gr */

#endif /* ADD_CONST_V_H */
