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

#ifndef INCLUDED_TAGGED_STREAM_ALIGN_H
#define INCLUDED_TAGGED_STREAM_ALIGN_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Align a stream to a tagged stream item
     * \ingroup stream_operators_blk
     *
     * \details
     * Takes an unaligned stream of tagged stream items and aligns to the first item
     */
    class BLOCKS_API tagged_stream_align : virtual public block
    {
     public:
      typedef boost::shared_ptr<tagged_stream_align> sptr;

      /*!
       * Make a tagged stream align
       *
       * \param itemsize The size (in bytes) of the item datatype.
       * \param lengthtagname Name of the TSB's length tag key.
       */
      static sptr make(size_t itemsize, const std::string &lengthtagname);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_TAGGED_STREAM_ALIGN_H */
