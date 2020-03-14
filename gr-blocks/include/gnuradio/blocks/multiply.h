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
    typedef std::shared_ptr<multiply<T>> sptr;

    static sptr make(size_t vlen = 1);
};

typedef multiply<std::int16_t> multiply_ss;
typedef multiply<std::int32_t> multiply_ii;
typedef multiply<float> multiply_ff;
typedef multiply<gr_complex> multiply_cc;
} /* namespace blocks */
} /* namespace gr */

#endif /* MULTIPLY_H */
