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

#ifndef INCLUDED_BLOCKS_KEEP_ONE_IN_N_H
#define INCLUDED_BLOCKS_KEEP_ONE_IN_N_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief decimate a stream, keeping the last item out of every \p n.
     * \ingroup stream_operators_blk
     */
    class BLOCKS_API keep_one_in_n : virtual public block
    {
    public:

      // gr::blocks::keep_one_in_n::sptr
      typedef boost::shared_ptr<keep_one_in_n> sptr;

      /*!
       * Make a keep one in n block.
       *
       * \param itemsize stream itemsize
       * \param n block size in items
       */
      static sptr make(size_t itemsize, int n);

      virtual void set_n(int n) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_KEEP_ONE_IN_N_H */
