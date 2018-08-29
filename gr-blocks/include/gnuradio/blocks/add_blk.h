/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
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


#ifndef ADD_BLK_H
#define ADD_BLK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
  namespace blocks {

    /*!
     * \brief output = sum(input[0], input[1], ..., input[M-1])
     * \ingroup math_operators_blk
     *
     * \details
     * Add samples across all input streams. For all \f$n\f$ samples
     * on all \f$M\f$ input streams \f$x_m\f$:
     *
     * \f[
     *   y[n] = \sum_{m=0}^{M-1} x_m[n]
     * \f]
     */
template<class T>
    class BLOCKS_API add_blk : virtual public sync_block
    {
    public:

      // gr::blocks::add_blk::sptr
      typedef boost::shared_ptr< add_blk<T> > sptr;

      static sptr make(size_t vlen=1);
    };

typedef add_blk<std::int16_t> add_ss;
typedef add_blk<std::int32_t> add_ii;
typedef add_blk<gr_complex> add_cc;
typedef add_blk<float> add_ff;
  } /* namespace blocks */
} /* namespace gr */

#endif /* ADD_BLK_H */
