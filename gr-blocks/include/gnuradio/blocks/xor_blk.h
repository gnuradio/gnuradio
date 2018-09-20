/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
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


#ifndef XOR_BLK_H
#define XOR_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
  namespace blocks {

    /*!
     * \brief output = input_0 ^ input_1 ^ , ... ^ input_N)
     * \ingroup boolean_operators_blk
     *
     * Bitwise boolean xor across all input streams.
     */
template<class T>
    class BLOCKS_API xor_blk : virtual public sync_block
    {
    public:

      // gr::blocks::xor::sptr
      typedef boost::shared_ptr< xor_blk<T> > sptr;

      static sptr make(size_t vlen=1);
    };

typedef xor_blk<std::uint8_t> xor_bb;
typedef xor_blk<std::int16_t> xor_ss;
typedef xor_blk<std::int32_t> xor_ii;
  } /* namespace blocks */
} /* namespace gr */

#endif /* XOR_BLK_H */
