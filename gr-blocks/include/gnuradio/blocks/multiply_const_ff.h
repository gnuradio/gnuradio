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

// @WARNING@

#ifndef INCLUDED_MULTIPLY_CONST_FF_H
#define INCLUDED_MULTIPLY_CONST_FF_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief output = input * real constant
     * \ingroup math_operators_blk
     */
    class BLOCKS_API multiply_const_ff : virtual public sync_block
    {

    public:

      // gr::blocks::multiply_const_ff::sptr
      typedef boost::shared_ptr<multiply_const_ff> sptr;

      /*!
       * \brief Create an instance of multiply_const_ff
       * \param k real multiplicative constant
       * \param vlen Vector length of incoming stream
       */
      static sptr make(float k, size_t vlen=1);

      /*!
       * \brief Return real multiplicative constant
       */
      virtual float k() const = 0;

      /*!
       * \brief Set real multiplicative constant
       */
      virtual void set_k(float k) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_MULTIPLY_CONST_FF_H */
