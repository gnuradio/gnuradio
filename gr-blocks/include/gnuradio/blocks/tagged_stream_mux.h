/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TAGGED_STREAM_MUX_H
#define INCLUDED_TAGGED_STREAM_MUX_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Combines tagged streams.
     * \ingroup stream_operators_blk
     *
     * \details
     * Takes N streams as input. Each stream is tagged with packet lengths.
     * Packets are output sequentially from each input stream.
     *
     * The output signal has a new length tag, which is the sum of all
     * individual length tags. The old length tags are discarded.
     *
     * All other tags are propagated as expected, i.e. they stay associated
     * with the same input item.
     */
    class BLOCKS_API tagged_stream_mux : virtual public tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<tagged_stream_mux> sptr;

      /*!
       * Make a tagged stream mux block.
       *
       * \param itemsize Items size (number of bytes per item)
       * \param lengthtagname Length tag key
       */
      static sptr make(size_t itemsize, const std::string &lengthtagname);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_TAGGED_STREAM_MUX_H */

