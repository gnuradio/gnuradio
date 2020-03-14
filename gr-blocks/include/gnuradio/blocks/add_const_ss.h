/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ADD_CONST_SS
#define ADD_CONST_SS

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief output = input + constant
 * \ingroup math_operators_blk
 */
class BLOCKS_API add_const_ss : virtual public sync_block
{
public:
    // gr::blocks::add_const_ss::sptr
    typedef std::shared_ptr<add_const_ss> sptr;

    /*!
     * \brief Create an instance of add_const_ss
     * \param k additive constant
     */
    static sptr make(short k);

    /*!
     * \brief Return additive constant
     */
    virtual short k() const = 0;

    /*!
     * \brief Set additive constant
     */
    virtual void set_k(short k) = 0;
};

} // namespace blocks
} // namespace gr

#endif /* ADD_CONST_SS */
