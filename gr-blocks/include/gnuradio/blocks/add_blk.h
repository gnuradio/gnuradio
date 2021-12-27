/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef ADD_BLK_H
#define ADD_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = sum(input[0], input[1], ..., input[M-1])
 * \ingroup math_operators_blk
 *
 * \details
 * Add samples across all input streams. For all \f$n\f$ samples
 * on all \f$M\f$ input streams \f$x_m\f$:
 *
 * \f[
 *   y[n] = \sum_{m=0}^{M-1} x_m[n]
 * \f]
 */
template <class T>
class BLOCKS_API add_blk : virtual public sync_block
{
public:
    // gr::blocks::add_blk::sptr
    using sptr = std::shared_ptr<add_blk<T>>;

    static sptr make(size_t vlen = 1);
};

using add_ss = add_blk<std::int16_t>;
using add_ii = add_blk<std::int32_t>;
using add_cc = add_blk<gr_complex>;
using add_ff = add_blk<float>;
} /* namespace blocks */
} /* namespace gr */

#endif /* ADD_BLK_H */
