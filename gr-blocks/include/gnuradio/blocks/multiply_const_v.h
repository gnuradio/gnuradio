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
    typedef std::shared_ptr<multiply_const_v<T>> sptr;

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

typedef multiply_const_v<std::int16_t> multiply_const_vss;
typedef multiply_const_v<std::int32_t> multiply_const_vii;
typedef multiply_const_v<float> multiply_const_vff;
typedef multiply_const_v<gr_complex> multiply_const_vcc;
} /* namespace blocks */
} /* namespace gr */

#endif /* MULTIPLY_CONST_V_H */
