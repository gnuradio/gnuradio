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

#ifndef INCLUDED_BLOCKS_DEINTERLEAVE_H
#define INCLUDED_BLOCKS_DEINTERLEAVE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief deinterleave a single input into N outputs
     * \ingroup stream_operators_blk
     */
    class BLOCKS_API deinterleave : virtual public sync_decimator
    {
    public:
      // gr::blocks::deinterleave::sptr
      typedef boost::shared_ptr<deinterleave> sptr;

      /*!
       * Make a deinterleave block.
       *
       * \param itemsize stream itemsize
       */
      static sptr make(size_t itemsize);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_DEINTERLEAVE_H */
