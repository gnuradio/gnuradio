/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TAGGED_STREAM_ALIGN_H
#define INCLUDED_TAGGED_STREAM_ALIGN_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief Align a stream to a tagged stream item
 * \ingroup stream_operators_blk
 *
 * \details
 * Takes an unaligned stream of tagged stream items and aligns to the first item
 */
class BLOCKS_API tagged_stream_align : virtual public block
{
public:
    typedef std::shared_ptr<tagged_stream_align> sptr;

    /*!
     * Make a tagged stream align
     *
     * \param itemsize The size (in bytes) of the item datatype.
     * \param lengthtagname Name of the TSB's length tag key.
     */
    static sptr make(size_t itemsize, const std::string& lengthtagname);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_TAGGED_STREAM_ALIGN_H */
