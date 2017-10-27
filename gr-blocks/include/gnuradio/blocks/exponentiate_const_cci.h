/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
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


#ifndef INCLUDED_BLOCKS_EXPONENTIATE_CONST_CCI_H
#define INCLUDED_BLOCKS_EXPONENTIATE_CONST_CCI_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Exponentiates a complex stream with an integer exponent
     * \ingroup blocks
     *
     * \details
     * This block raises a complex stream to an integer exponent. The exponent
     * must be at least 1. There is a callback function so the exponent can be
     * changed at runtime.
     *
     * NOTE: The algorithm uses iterative multiplication to achieve exponentiation, 
     * hence it is O(exponent). Therefore, this block could be inefficient for large
     * exponents.
     */
    class BLOCKS_API exponentiate_const_cci : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<exponentiate_const_cci> sptr;

      /*
       * \param exponent Exponent the input stream is raised to, which must be an integer.
       * The algorithm uses iterative multiplication to achieve exponentiation, hence it is
       * O(exponent). Therefore, this block could be inefficient for large exponents.
       * \param vlen Vector length of input/output stream
       */
      static sptr make(int exponent, size_t vlen = 1);

      virtual void set_exponent(int exponent) = 0;
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_EXPONENTIATE_CONST_CCI_H */

