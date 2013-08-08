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

#ifndef INCLUDED_BLOCKS_SHORT_TO_FLOAT_H
#define INCLUDED_BLOCKS_SHORT_TO_FLOAT_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Convert stream of shorts to a stream of floats
     * \ingroup type_converters_blk
     */
    class BLOCKS_API short_to_float : virtual public sync_block
    {
    public:
      // gr::blocks::short_to_float_ff::sptr
      typedef boost::shared_ptr<short_to_float> sptr;

      /*!
       * Build a short to float block.
       *
       * \param vlen vector length of data streams.
       * \param scale a scalar divider to change the output signal scale.
       */
      static sptr make(size_t vlen=1, float scale=1.0);

      /*!
       * Get the scalar divider value.
       */
      virtual float scale() const = 0;

      /*!
       * Set the scalar divider value.
       */
      virtual void set_scale(float scale) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_SHORT_TO_FLOAT_H */
