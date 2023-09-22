/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_STREAMS_TO_VECTOR_H
#define INCLUDED_BLOCKS_STREAMS_TO_VECTOR_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief convert N streams of items to 1 stream of vector length N
 * \ingroup stream_operators_blk
 */
class BLOCKS_API streams_to_vector : virtual public sync_block
{
public:
    // gr::blocks::streams_to_vector::sptr
    typedef std::shared_ptr<streams_to_vector> sptr;

    /*!
     * Make a stream-to-vector block.
     *
     * \param itemsize the item size of the stream
     * \param nstreams number of streams to combine into a vector (vector size)
     */
    static sptr make(size_t itemsize, size_t nstreams);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_STREAMS_TO_VECTOR_H */
