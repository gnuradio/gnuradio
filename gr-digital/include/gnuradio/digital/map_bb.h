/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_MAP_BB_H
#define INCLUDED_GR_MAP_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief output[i] = map[input[i]]
     * \ingroup symbol_coding_blk
     *
     * \details
     * This block maps an incoming signal to the value in the map.
     * The block expects that the incoming signal has a maximum
     * value of len(map)-1.
     *
     * -> output[i] = map[input[i]]
     */
    class DIGITAL_API map_bb : virtual public sync_block
    {
    public:
      // gr::digital::map_bb::sptr
      typedef boost::shared_ptr<map_bb> sptr;
      
      /*!
       * Make a map block.
       *
       * \param map a vector of integers that maps x to map[x].
       */
      static sptr make(const std::vector<int> &map);

      virtual void set_map(const std::vector<int> &map) = 0;
      virtual std::vector<int> map() const = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_MAP_BB_H */
