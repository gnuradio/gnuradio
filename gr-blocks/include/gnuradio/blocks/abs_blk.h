/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef ABS_BLK_H
#define ABS_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output[m] = abs(input[m]) for all M streams.
 * \ingroup boolean_operators_blk
 *
 * \details
 * absolute value of data stream (Strip sign)
 */
template <class T>
class BLOCKS_API abs_blk : virtual public sync_block
{

public:
    // gr::blocks::abs_blk::sptr
    typedef std::shared_ptr<abs_blk<T>> sptr;

    /*!
     * \brief Create an instance of abs_blk
     */
    static sptr make(size_t vlen = 1);
};

typedef abs_blk<std::int16_t> abs_ss;
typedef abs_blk<std::int32_t> abs_ii;
typedef abs_blk<float> abs_ff;
} /* namespace blocks */
} /* namespace gr */

#endif /* ABS_BLK_H */
