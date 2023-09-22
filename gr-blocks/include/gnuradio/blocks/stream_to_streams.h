/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_STREAM_TO_STREAMS_H
#define INCLUDED_BLOCKS_STREAM_TO_STREAMS_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace blocks {

/*!
 * \brief convert a stream of items into a N streams of items
 * \ingroup stream_operators_blk
 *
 * \details
 * Converts a stream of N items into N streams of 1 item.
 * Repeat ad infinitum.
 */
class BLOCKS_API stream_to_streams : virtual public sync_decimator
{
public:
    // gr::blocks::stream_to_streams::sptr
    typedef std::shared_ptr<stream_to_streams> sptr;

    /*!
     * Make a stream-to-streams block.
     *
     * \param itemsize the item size of the stream
     * \param nstreams number of streams to split input into
     */
    static sptr make(size_t itemsize, size_t nstreams);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_STREAM_TO_STREAMS_H */
