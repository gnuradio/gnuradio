/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef MULTIPLY_CONST_V_H
#define MULTIPLY_CONST_V_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = input * constant vector (element-wise)
 * \ingroup math_operators_blk
 */
template <class T>
class BLOCKS_API multiply_const_v : virtual public sync_block
{

public:
    // gr::blocks::multiply_const_v::sptr
    using sptr = std::shared_ptr<multiply_const_v<T>>;

    /*!
     * \brief Create an instance of multiply_const_v
     * \param k multiplicative constant vector
     */
    static sptr make(std::vector<T> k);

    /*!
     * \brief Return multiplicative constant vector
     */
    virtual std::vector<T> k() const = 0;

    /*!
     * \brief Set multiplicative constant vector
     */
    virtual void set_k(std::vector<T> k) = 0;
};

using multiply_const_vss = multiply_const_v<std::int16_t>;
using multiply_const_vii = multiply_const_v<std::int32_t>;
using multiply_const_vff = multiply_const_v<float>;
using multiply_const_vcc = multiply_const_v<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

#endif /* MULTIPLY_CONST_V_H */
