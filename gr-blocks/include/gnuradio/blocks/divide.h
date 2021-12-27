/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef DIVIDE_H
#define DIVIDE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = input[0] / input[1] / ... / input[M-1]
 * \ingroup math_operators_blk
 *
 * \details
 * Divide across all input streams.
 */
template <class T>
class BLOCKS_API divide : virtual public sync_block
{
public:
    // gr::blocks::divide::sptr
    using sptr = std::shared_ptr<divide<T>>;

    static sptr make(size_t vlen = 1);
};

using divide_ss = divide<std::int16_t>;
using divide_ii = divide<std::int32_t>;
using divide_ff = divide<float>;
using divide_cc = divide<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

#endif /* DIVIDE_H */
