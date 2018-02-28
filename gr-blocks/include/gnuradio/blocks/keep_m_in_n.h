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

#ifndef INCLUDED_BLOCKS_KEEP_M_IN_N_H
#define INCLUDED_BLOCKS_KEEP_M_IN_N_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief decimate a stream, keeping the first \p m items out of every \p n starting after \p offset items.
     * \ingroup stream_operators_blk
     */
    class BLOCKS_API keep_m_in_n : virtual public block
    {
    public:

      // gr::blocks::keep_m_in_n::sptr
      typedef boost::shared_ptr<keep_m_in_n> sptr;

      /*!
       * Make a keep m in n block.
       *
       * \param itemsize stream itemsize
       * \param m number of items to take in block of \p n items
       * \param n block size in items
       * \param offset initial item offset into the stream
       */
      static sptr make(size_t itemsize, int m, int n, int offset);

      virtual void set_m(int m) = 0;
      virtual void set_n(int n) = 0;
      virtual void set_offset(int offset) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_KEEP_M_IN_N_H */
