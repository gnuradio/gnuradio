/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_VECTOR_TO_STREAM_H
#define INCLUDED_BLOCKS_VECTOR_TO_STREAM_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace blocks {

/*!
 * \brief convert a stream of gnuradio/blocks of nitems_per_block items into a stream of
 * items \ingroup stream_operators_blk
 */
class BLOCKS_API vector_to_stream : virtual public sync_interpolator
{
public:
    // gr::blocks::vector_to_stream::sptr
    typedef std::shared_ptr<vector_to_stream> sptr;

    /*!
     * Make vector-to-stream block
     *
     * \param itemsize the item size of the stream
     * \param nitems_per_block number of items per vector (vector size)
     */
    static sptr make(size_t itemsize, size_t nitems_per_block);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_VECTOR_TO_STREAM_H */
