/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    using sptr = std::shared_ptr<multiply_const<T>>;

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

using multiply_const_ss = multiply_const<std::int16_t>;
using multiply_const_ii = multiply_const<std::int32_t>;
using multiply_const_ff = multiply_const<float>;
using multiply_const_cc = multiply_const<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

#endif /* MULTIPLY_CONST_H */
