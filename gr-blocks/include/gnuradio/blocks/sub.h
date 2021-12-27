/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef SUB_H
#define SUB_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = input_0 -  input_1 - ...)
 * \ingroup math_operators_blk
 *
 * \details
 * Subtract across all input streams.
 */
template <class T>
class BLOCKS_API sub : virtual public sync_block
{
public:
    // gr::blocks::sub::sptr
    using sptr = std::shared_ptr<sub<T>>;

    static sptr make(size_t vlen = 1);
};

using sub_ss = sub<std::int16_t>;
using sub_ii = sub<std::int32_t>;
using sub_cc = sub<gr_complex>;
using sub_ff = sub<float>;
} /* namespace blocks */
} /* namespace gr */

#endif /* SUB_H */
