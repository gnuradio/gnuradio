/* -*- c++ -*- */
/*
 * Copyright 2008,2013,2017,2018 Free Software Foundation, Inc.
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


#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
  namespace blocks {

    /*!
     * \brief output is the moving sum of the last N samples, scaled by the scale factor
     * \ingroup level_controllers_blk
     */
template<class T>
    class BLOCKS_API moving_average : virtual public sync_block
    {
    public:
      // gr::blocks::moving_average::sptr
      typedef boost::shared_ptr< moving_average<T> > sptr;

      /*!
       * Create a moving average block.
       *
       * \param length Number of samples to use in the average.
       * \param scale scale factor for the result.
       * \param max_iter limits how long we go without flushing the accumulator
       *        This is necessary to avoid numerical instability for float and complex.
       * \param vlen When > 1, do a per-vector-element moving average
       */
      static sptr make(int length, T scale,
                       int max_iter = 4096,
                       unsigned int vlen = 1);

      /*!
       * Get the length used in the avaraging calculation.
       */
      virtual int length() const = 0;

      /*!
       * Get the scale factor being used.
       */
      virtual T scale() const = 0;

      /*!
       * Set both the length and the scale factor together.
       */
      virtual void set_length_and_scale(int length, T scale) = 0;

      /*!
       * Set the length.
       */
      virtual void set_length(int length) = 0;

      /*!
       * Set the scale factor.
       */
      virtual void set_scale(T scale) = 0;
    };

typedef moving_average<std::int16_t> moving_average_ss;
typedef moving_average<std::int32_t> moving_average_ii;
typedef moving_average<float> moving_average_ff;
typedef moving_average<gr_complex> moving_average_cc;
  } /* namespace blocks */
} /* namespace gr */

#endif /* MOVING_AVERAGE_H */
