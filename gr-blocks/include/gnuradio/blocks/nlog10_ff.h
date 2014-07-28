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

#ifndef INCLUDED_BLOCKS_NLOG10_FF_H
#define INCLUDED_BLOCKS_NLOG10_FF_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief output = n*log10(input) + k
     * \ingroup math_operators_blk
     */
    class BLOCKS_API nlog10_ff : virtual public sync_block
    {
    public:

      // gr::blocks::nlog10_ff::sptr
      typedef boost::shared_ptr<nlog10_ff> sptr;

      /*!
       * \brief Make an instance of an nlog10_ff block.
       * \param n     Scalar multiplicative constant
       * \param vlen  Input vector length
       * \param k     Scalar additive constant
       */
      static sptr make(float n=1.0, size_t vlen=1, float k=0.0);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_NLOG10_FF_H */
