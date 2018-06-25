/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_INTERLEAVE_H
#define INCLUDED_BLOCKS_INTERLEAVE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief interleave N inputs into a single output
     * \ingroup stream_operators_blk
     *
     * \details
     *
     * This block interleaves blocks of samples. For each input
     * connection, the samples are interleaved successively to the
     * output connection. By default, the block interleaves a single
     * sample from each input to the output unless blocksize is given
     * in the constructor.
     *
     * \code
     *   blocksize = 1
     *   connections = 2
     *   input[0] = [a, c, e, g]
     *   input[1] = [b, d, f, h]
     *   output = [a, b, c, d, e, f, g, h]
     * \endcode
     *
     * \code
     *   blocksize = 2
     *   connections = 2
     *   input[0] = [a, b, e, f]
     *   input[1] = [c, d, g, h]
     *   output = [a, b, c, d, e, f, g, h]
     * \endcode
     */
    class BLOCKS_API interleave : virtual public block
    {
    public:
      // gr::blocks::interleave::sptr
      typedef boost::shared_ptr<interleave> sptr;

      /*!
       * Make a stream interleave block.
       *
       * \param itemsize stream itemsize
       * \param blocksize size of block of samples to interleave
       */
      static sptr make(size_t itemsize, unsigned int blocksize = 1);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_INTERLEAVE_H */
