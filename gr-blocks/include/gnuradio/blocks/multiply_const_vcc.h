/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_MULTIPLY_CONST_VCC_H
#define INCLUDED_MULTIPLY_CONST_VCC_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief output = input * constant vector (element-wise)
     * \ingroup math_operators_blk
     */
    class BLOCKS_API multiply_const_vcc : virtual public sync_block
    {
    public:
      // gr::blocks::multiply_const_vcc::sptr
      typedef boost::shared_ptr<multiply_const_vcc> sptr;

      /*!
       * \brief Create an instance of multiply_const_vcc
       * \param k multiplicative constant vector
       */
      static sptr make(std::vector<gr_complex> k);

      /*!
       * \brief Return multiplicative constant vector
       */
      virtual std::vector<gr_complex> k() const = 0;

      /*!
       * \brief Set multiplicative constant vector
       */
      virtual void set_k(std::vector<gr_complex> k) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_MULTIPLY_CONST_VCC_H */
