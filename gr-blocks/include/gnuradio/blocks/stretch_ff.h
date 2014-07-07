/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_STRETCH_FF_H
#define INCLUDED_GR_STRETCH_FF_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief adjust y-range of an input vector by mapping to range
     * (max-of-input, stipulated-min). Primarily for spectral
     * signature matching by normalizing spectrum dynamic ranges.
     * \ingroup stream_operators_blk
     */
    class BLOCKS_API stretch_ff : virtual public sync_block
    {
    public:
      // gr::blocks::stretch_ff::sptr
      typedef boost::shared_ptr<stretch_ff> sptr;

      /*!
       * \brief Make a stretch block.
       *
       * \param lo Set low value for range.
       * \param vlen vector length of input stream.
       */
      static sptr make(float lo, size_t vlen=1);

      virtual float lo() const = 0;
      virtual void set_lo(float lo) = 0;
      virtual size_t vlen() const = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_STRETCH_FF_H */
