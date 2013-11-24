/* -*- c++ -*- */
/*
 * Copyright 2007,2013 Free Software Foundation, Inc.
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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Compares vectors from multiple streams and determines
     * the index in the vector and stream number where the maximum
     * value occurred.
     * \ingroup math_operators_blk
     *
     * \details
     * Data is passed in as a vector of length \p vlen from multiple
     * input sources. It will look through these streams of \p vlen
     * data items and output two streams:
     *
     * \li Stream 0 will contain the index value in the vector where
     *     the maximum value occurred.
     *
     * \li Stream 1 will contain the number of the input stream that
     *     held the maximum value.
     */
    class BLOCKS_API @NAME@ : virtual public sync_block
    {
    public:
      // gr::blocks::@NAME@::sptr
      typedef boost::shared_ptr<@NAME@> sptr;

      static sptr make(size_t vlen);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
