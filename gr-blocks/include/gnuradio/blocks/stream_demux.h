/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_BLOCKS_STREAM_DEMUX_H
#define INCLUDED_BLOCKS_STREAM_DEMUX_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>
#include <vector>

namespace gr {
namespace blocks {

/*!
 * \brief Stream demuxing block to demultiplex one stream into N output streams
 * \ingroup stream_operators_blk
 *
 * \details
 * Demuxes a stream producing N outputs streams that contains n_0 items in
 * the first stream, n_1 items in the second, etc. and repeats. Number of
 * items of each output stream is specified using the 'lengths' parameter
 * like so [n_0, n_1, ..., n_N-1].
 *
 * Example:
 * lengths = [2, 3, 4]
 * input stream: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, ...]
 * output_streams: [0, 1, 9, 10, ...]
 *                 [2, 3, 4, 11, ...]
 *                 [5, 6, 7, 8, ...]
 */
class BLOCKS_API stream_demux : virtual public gr::block
{
public:
    // gr::blocks::stream_demux::sptr
    typedef std::shared_ptr<stream_demux> sptr;

    /*!
     * \brief Stream demuxing block to demultiplex one stream into N output streams
     *
     * \param itemsize the item size of the stream
     * \param lengths  a vector (list/tuple) specifying the number of
     *                 items to copy to each output stream.
     *
     */
    static sptr make(size_t itemsize, const std::vector<int>& lengths);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_STREAM_DEMUX_H */
