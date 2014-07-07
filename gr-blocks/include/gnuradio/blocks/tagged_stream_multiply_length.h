/* -*- c++ -*- */
/* Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_H
#define INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Allows scaling of a tagged stream length tag
     * \ingroup stream_operators_blk
     *
     * \details
     * Searches for a specific tagged stream length tag and multiplies
     * that length by a constant - for constant rate change blocks
     * in a tagged stream
     */
    class BLOCKS_API tagged_stream_multiply_length : virtual public block
    {
     public:
      typedef boost::shared_ptr<tagged_stream_multiply_length> sptr;
      virtual void set_scalar(double scalar) = 0;

      /*!
       * Make a tagged stream multiply_length block.
       *
       * \param itemsize Items size (number of bytes per item)
       * \param lengthtagname Length tag key
       * \param scalar value to scale length tag values by
       */
      static sptr make(size_t itemsize, const std::string &lengthtagname, double scalar);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_H */

