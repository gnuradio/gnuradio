/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_NLOG10_FF_H
#define INCLUDED_BLOCKS_NLOG10_FF_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief output = n*log10(input) + k
 * \ingroup math_operators_blk
 */
class BLOCKS_API nlog10_ff : virtual public sync_block
{
public:
    // gr::blocks::nlog10_ff::sptr
    typedef std::shared_ptr<nlog10_ff> sptr;

    /*!
     * \brief Make an instance of an nlog10_ff block.
     * \param n     Scalar multiplicative constant
     * \param vlen  Input vector length
     * \param k     Scalar additive constant
     */
    static sptr make(float n = 1.0, size_t vlen = 1, float k = 0.0);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_NLOG10_FF_H */
