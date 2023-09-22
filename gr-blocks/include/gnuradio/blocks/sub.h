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
    typedef std::shared_ptr<sub<T>> sptr;

    static sptr make(size_t vlen = 1);
};

typedef sub<std::int16_t> sub_ss;
typedef sub<std::int32_t> sub_ii;
typedef sub<gr_complex> sub_cc;
typedef sub<float> sub_ff;
} /* namespace blocks */
} /* namespace gr */

#endif /* SUB_H */
