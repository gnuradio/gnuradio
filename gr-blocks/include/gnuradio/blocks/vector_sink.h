/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2009,2013,2018 Free Software Foundation, Inc.
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


#ifndef VECTOR_SINK_H
#define VECTOR_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
  namespace blocks {

    /*!
     * \brief T sink that writes to a vector
     * \ingroup debug_tools_blk
     */
template<class T>
    class BLOCKS_API vector_sink : virtual public sync_block
    {
    public:
      // gr::blocks::vector_sink::sptr
      typedef boost::shared_ptr< vector_sink<T> > sptr;

      /*!
       * \brief Make a new instance of the vector source, and return a shared pointer to it.
       * \param vlen length of vector items
       * \param reserve_items reserve space in the internal storage for this many items;
       *                      the internal storage will still grow to accommodate more item
       *                      if necessary, but setting this to a realistic value can avoid
       *                      memory allocations during runtime, especially if you know a
       *                      priori how many items you're going to store.
       */
      static sptr make(const unsigned int vlen = 1, const int reserve_items = 1024);

      //! Clear the data and tags containers.
      virtual void reset() = 0;
      virtual std::vector<T> data() const = 0;
      virtual std::vector<tag_t> tags() const = 0;
    };

typedef vector_sink<std::uint8_t> vector_sink_b;
typedef vector_sink<std::int16_t> vector_sink_s;
typedef vector_sink<std::int32_t> vector_sink_i;
typedef vector_sink<float> vector_sink_f;
typedef vector_sink<gr_complex> vector_sink_c;
  } /* namespace blocks */
} /* namespace gr */

#endif /* VECTOR_SINK_H */
