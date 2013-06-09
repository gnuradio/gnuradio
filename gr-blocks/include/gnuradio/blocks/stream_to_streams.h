/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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
      typedef boost::shared_ptr<stream_to_streams> sptr;

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
