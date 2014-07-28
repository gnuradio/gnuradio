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

#ifndef INCLUDED_BLOCKS_STREAM_MUX_H
#define INCLUDED_BLOCKS_STREAM_MUX_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>
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
      typedef boost::shared_ptr<stream_mux> sptr;

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
      static sptr make(size_t itemsize, const std::vector<int> &lengths);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_STREAM_MUX_H */
