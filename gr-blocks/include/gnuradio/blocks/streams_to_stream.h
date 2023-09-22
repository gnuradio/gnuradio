/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_STREAMS_TO_STREAM_H
#define INCLUDED_BLOCKS_STREAMS_TO_STREAM_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert N streams of 1 item into a 1 stream of N items
 * \ingroup stream_operators_blk
 *
 * \details
 * Convert N streams of 1 item into 1 stream of N items.
 * Repeat ad infinitum.
 */
class BLOCKS_API streams_to_stream : virtual public sync_interpolator
{
public:
    // gr::blocks::streams_to_stream::sptr
    typedef std::shared_ptr<streams_to_stream> sptr;

    /*!
     * Make a streams-to-stream block.
     *
     * \param itemsize the item size of the stream
     * \param nstreams number of streams to combine
     */
    static sptr make(size_t itemsize, size_t nstreams);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_STREAMS_TO_STREAM_H */
