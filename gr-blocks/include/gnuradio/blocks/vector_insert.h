/* -*- c++ -*- */
/*
 * Copyright 2012-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef VECTOR_INSERT_H
#define VECTOR_INSERT_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief source of T's that gets its data from a vector
 * \ingroup stream_operators_blk
 */
template <class T>
class BLOCKS_API vector_insert : virtual public block
{
public:
    // gr::blocks::vector_insert::sptr
    using sptr = std::shared_ptr<vector_insert<T>>;

    /*!
     * Make vector insert block.
     *
     * \param data vector of data to insert
     * \param periodicity number of samples between when to send \p data
     * \param offset initial item offset of first insert
     */
    static sptr make(const std::vector<T>& data, int periodicity, int offset = 0);

    virtual void rewind() = 0;
    virtual void set_data(const std::vector<T>& data) = 0;
};

using vector_insert_b = vector_insert<std::uint8_t>;
using vector_insert_s = vector_insert<std::int16_t>;
using vector_insert_i = vector_insert<std::int32_t>;
using vector_insert_f = vector_insert<float>;
using vector_insert_c = vector_insert<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

#endif /* VECTOR_INSERT_H */
