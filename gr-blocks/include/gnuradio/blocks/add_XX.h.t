/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012 Free Software Foundation, Inc.
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
    class BLOCKS_API @NAME@ : virtual public sync_block
    {
    public:

      // gr::blocks::@NAME@::sptr
      typedef boost::shared_ptr<@NAME@> sptr;

      static sptr make(size_t vlen=1);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
