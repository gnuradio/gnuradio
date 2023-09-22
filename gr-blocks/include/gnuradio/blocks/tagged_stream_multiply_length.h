/* -*- c++ -*- */
/* Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_H
#define INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Allows scaling of a tagged stream length tag
 * \ingroup stream_operators_blk
 *
 * \details
 * Searches for a specific tagged stream length tag and multiplies
 * that length by a constant - for constant rate change blocks
 * in a tagged stream
 */
class BLOCKS_API tagged_stream_multiply_length : virtual public block
{
public:
    typedef std::shared_ptr<tagged_stream_multiply_length> sptr;
    virtual void set_scalar(double scalar) = 0;

    /*!
     * Make a tagged stream multiply_length block.
     *
     * \param itemsize Items size (number of bytes per item)
     * \param lengthtagname Length tag key
     * \param scalar value to scale length tag values by
     */
    static sptr make(size_t itemsize, const std::string& lengthtagname, double scalar);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_H */
