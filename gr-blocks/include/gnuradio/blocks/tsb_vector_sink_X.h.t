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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gnuradio/blocks/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief A vector sink for tagged streams.
     *
     * Unlike a gr::blocks::vector_sink_f, this only works with tagged streams.
     *
     * \ingroup blocks
     */
    class BLOCKS_API @NAME@ : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<@NAME@> sptr;

      virtual void reset() = 0;
      virtual std::vector<std::vector<@TYPE@> > data() const = 0;
      /*! Doesn't include the TSB tags.
       */
      virtual std::vector<tag_t> tags() const = 0;

      /*!
       * \param vlen Vector length
       * \param tsb_key Tagged Stream Key
       */
      static sptr make(int vlen=1, const std::string &tsb_key="ts_last");
    };

  } // namespace blocks
} // namespace gr

#endif /* @GUARD_NAME@ */

