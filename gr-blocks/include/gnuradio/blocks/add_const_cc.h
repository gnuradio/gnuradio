/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2015 Free Software Foundation, Inc.
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

#ifndef ADD_CONST_CC
#define ADD_CONST_CC

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief output = input + constant
     * \ingroup math_operators_blk
     */
    class BLOCKS_API add_const_cc : virtual public sync_block
    {
    public:

      // gr::blocks::add_const_cc::sptr
      typedef boost::shared_ptr<add_const_cc> sptr;

      /*!
       * \brief Create an instance of add_const_cc
       * \param k additive constant
       */
      static sptr make(gr_complex k);

      /*!
       * \brief Return additive constant
       */
      virtual gr_complex k() const = 0;

      /*!
       * \brief Set additive constant
       */
      virtual void set_k(gr_complex k) = 0;
    };

  }
}

#endif /* ADD_CONST_CC */
