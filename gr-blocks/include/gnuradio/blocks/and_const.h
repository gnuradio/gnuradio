/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
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


#ifndef AND_CONST_H
#define AND_CONST_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
  namespace blocks {

    /*!
     * \brief output[m] = input[m] & value for all M streams.
     * \ingroup boolean_operators_blk
     *
     * \details
     * Bitwise boolean AND of constant \p k with the data stream.
     */
template<class T>
    class BLOCKS_API and_const : virtual public sync_block
    {

    public:

      // gr::blocks::and_const::sptr
      typedef boost::shared_ptr< and_const<T> > sptr;

      /*!
       * \brief Create an instance of and_const
       * \param k AND constant
       */
      static sptr make(T k);

      /*!
       * \brief Return AND constant
       */
      virtual T k() const = 0;

      /*!
       * \brief Set AND constant
       */
      virtual void set_k(T k) = 0;
    };

typedef and_const<std::uint8_t> and_const_bb;
typedef and_const<std::int16_t> and_const_ss;
typedef and_const<std::int32_t> and_const_ii;
  } /* namespace blocks */
} /* namespace gr */

#endif /* AND_CONST_H */
