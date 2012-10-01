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

#ifndef INCLUDED_BLOCKS_STREAMS_TO_STREAM_H
#define INCLUDED_BLOCKS_STREAMS_TO_STREAM_H

#include <blocks/api.h>
#include <gr_sync_interpolator.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Convert N streams of 1 item into a 1 stream of N items
     * \ingroup slicedice_blk
     *
     * Convert N streams of 1 item into 1 stream of N items.
     * Repeat ad infinitum.
     */
    class BLOCKS_API streams_to_stream : virtual public gr_sync_interpolator
    {
    public:
      
      // gr::blocks::streams_to_stream::sptr
      typedef boost::shared_ptr<streams_to_stream> sptr;

      static sptr make(size_t itemsize, size_t nstreams);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_STREAMS_TO_STREAM_H */
