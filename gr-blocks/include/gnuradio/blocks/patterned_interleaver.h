/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_PATTERNED_INTERLEAVER_H
#define INCLUDED_BLOCKS_PATTERNED_INTERLEAVER_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Interleave items based on the provided vector \p pattern.
     * \ingroup stream_operators_blk
     */
    class BLOCKS_API patterned_interleaver : virtual public block
    {
    public:
      typedef boost::shared_ptr<patterned_interleaver> sptr;

      /*!
       * Make a patterned interleaver block.
       *
       * \param itemsize stream itemsize
       * \param pattern vector that represents the interleaving pattern
       */
      static sptr make(size_t itemsize, std::vector<int> pattern);
    };

  }
}

#endif /* INCLUDED_BLOCKS_PATTERNED_INTERLEAVER_H */

