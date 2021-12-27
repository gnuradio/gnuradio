/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ARGMAX_H
#define ARGMAX_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Compares vectors from multiple streams and determines
 * the index in the vector and stream number where the maximum
 * value occurred.
 * \ingroup math_operators_blk
 *
 * \details
 * Data is passed in as a vector of length \p vlen from multiple
 * input sources. It will look through these streams of \p vlen
 * data items and output two streams:
 *
 * \li Stream 0 will contain the index value in the vector where
 *     the maximum value occurred.
 *
 * \li Stream 1 will contain the number of the input stream that
 *     held the maximum value.
 */
template <class T>
class BLOCKS_API argmax : virtual public sync_block
{
public:
    using sptr = std::shared_ptr<argmax<T>>;

    static sptr make(size_t vlen);
};

using argmax_fs = argmax<float>;
using argmax_is = argmax<std::int32_t>;
using argmax_ss = argmax<std::int16_t>;
} /* namespace blocks */
} /* namespace gr */

#endif /* ARGMAX_H */
