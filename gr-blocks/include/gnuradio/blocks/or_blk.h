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

#ifndef OR_BLK_H
#define OR_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief output = input_0 | input_1 | , ... | input_N)
     * \ingroup boolean_operators_blk
     *
     * Bitwise boolean or across all input streams.
     */
    template<class T>
    class BLOCKS_API or_blk : virtual public sync_block
    {
    public:

      // gr::blocks::or_blk::sptr
      typedef boost::shared_ptr<or_blk<T> > sptr;

      static sptr make(size_t vlen=1);
    };

    typedef or_blk<short> or_ss;
    typedef or_blk<int> or_ii;
    typedef or_blk<char> or_bb;
  } /* namespace blocks */
} /* namespace gr */

#endif /* OR_BLK_H */
