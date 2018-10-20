/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
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


#ifndef INTEGRATE_H
#define INTEGRATE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_decimator.h>
#include <cstdint>

namespace gr {
  namespace blocks {

    /*!
     * \brief Integrate successive samples and decimate
     * \ingroup math_operators_blk
     */
template<class T>
    class BLOCKS_API integrate : virtual public sync_decimator
    {
    public:

      // gr::blocks::integrate::sptr
      typedef boost::shared_ptr< integrate<T> > sptr;

      static sptr make(int decim, unsigned int vlen = 1);
    };

typedef integrate<std::int16_t> integrate_ss;
typedef integrate<std::int32_t> integrate_ii;
typedef integrate<float> integrate_ff;
typedef integrate<gr_complex> integrate_cc;
  } /* namespace blocks */
} /* namespace gr */

#endif /* INTEGRATE_H */
