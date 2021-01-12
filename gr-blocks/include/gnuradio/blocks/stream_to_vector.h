/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_STREAM_TO_VECTOR_H
#define INCLUDED_BLOCKS_STREAM_TO_VECTOR_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace blocks {

/*!
 * \brief convert a stream of items into a stream of gnuradio/blocks containing
 * nitems_per_block \ingroup stream_operators_blk
 */
class BLOCKS_API stream_to_vector : virtual public sync_decimator
{
public:
    // gr::blocks::stream_to_vector::sptr
    typedef std::shared_ptr<stream_to_vector> sptr;

    /*!
     * Make a stream-to-vector block.
     *
     * \param itemsize the item size of the stream
     * \param nitems_per_block number of items to put into each vector (vector size)
     */
    static sptr make(size_t itemsize, size_t nitems_per_block);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_STREAM_TO_VECTOR_H */
