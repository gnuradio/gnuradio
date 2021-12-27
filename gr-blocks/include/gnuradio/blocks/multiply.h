/* -*- c++ -*- */
/*
 * Copyright 2004, 2009, 2012, 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    using sptr = std::shared_ptr<multiply<T>>;

    static sptr make(size_t vlen = 1);
};

using multiply_ss = multiply<std::int16_t>;
using multiply_ii = multiply<std::int32_t>;
using multiply_ff = multiply<float>;
using multiply_cc = multiply<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

#endif /* MULTIPLY_H */
