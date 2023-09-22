/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_STREAM_MUX_H
#define INCLUDED_BLOCKS_STREAM_MUX_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>
#include <vector>

namespace gr {
namespace blocks {

/*!
 * \brief Stream muxing block to multiplex many streams into
 * one with a specified format.
 * \ingroup stream_operators_blk
 *
 * \details
 * Muxes N streams together producing an output stream that
 * contains N0 items from the first stream, N1 items from the second,
 * etc. and repeats:
 *
 * [N0, N1, N2, ..., Nm, N0, N1, ...]
 */
class BLOCKS_API stream_mux : virtual public block
{
public:
    // gr::blocks::stream_mux::sptr
    typedef std::shared_ptr<stream_mux> sptr;

    /*!
     * \brief Creates a stream muxing block to multiplex many streams into
     * one with a specified format.
     *
     * \param itemsize the item size of the stream
     * \param lengths  a vector (list/tuple) specifying the number of
     *                 items from each stream the mux together.
     *                 Warning: this requires that at least as many items
     *                 per stream are available or the system will wait
     *                 indefinitely for the items.
     *
     */
    static sptr make(size_t itemsize, const std::vector<int>& lengths);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_STREAM_MUX_H */
