/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_VECTOR_TO_STREAMS_H
#define INCLUDED_BLOCKS_VECTOR_TO_STREAMS_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert 1 stream of vectors of length N to N streams of items
 * \ingroup stream_operators_blk
 */
class BLOCKS_API vector_to_streams : virtual public sync_block
{
public:
    // gr::blocks::vector_to_streams::sptr
    typedef std::shared_ptr<vector_to_streams> sptr;

    /*!
     * Make vector-to-streams block
     *
     * \param itemsize the item size of the stream
     * \param nstreams number of items per vector (vector size and
     *                 number of streams produced)
     */
    static sptr make(size_t itemsize, size_t nstreams);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_VECTOR_TO_STREAMS_H */
