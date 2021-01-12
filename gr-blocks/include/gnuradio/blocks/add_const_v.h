/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    typedef std::shared_ptr<add_const_v<T>> sptr;

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
