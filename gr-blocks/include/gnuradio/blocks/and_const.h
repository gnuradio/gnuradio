/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef AND_CONST_H
#define AND_CONST_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output[m] = input[m] & value for all M streams.
 * \ingroup boolean_operators_blk
 *
 * \details
 * Bitwise boolean AND of constant \p k with the data stream.
 */
template <class T>
class BLOCKS_API and_const : virtual public sync_block
{

public:
    // gr::blocks::and_const::sptr
    using sptr = std::shared_ptr<and_const<T>>;

    /*!
     * \brief Create an instance of and_const
     * \param k AND constant
     */
    static sptr make(T k);

    /*!
     * \brief Return AND constant
     */
    virtual T k() const = 0;

    /*!
     * \brief Set AND constant
     */
    virtual void set_k(T k) = 0;
};

using and_const_bb = and_const<std::uint8_t>;
using and_const_ss = and_const<std::int16_t>;
using and_const_ii = and_const<std::int32_t>;
} /* namespace blocks */
} /* namespace gr */

#endif /* AND_CONST_H */
