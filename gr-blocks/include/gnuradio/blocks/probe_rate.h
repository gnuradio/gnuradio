/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_PROBE_RATE_H
#define INCLUDED_BLOCKS_PROBE_RATE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief throughput measurement
     * \ingroup measurement_tools_blk
     */
    class BLOCKS_API probe_rate : virtual public sync_block
    {
    public:
      // gr::blocks::probe_rate::sptr
      typedef boost::shared_ptr<probe_rate> sptr;

      /*!
       * \brief Make a throughput measurement block
       * \param itemsize size of each stream item
       * \param update_rate_ms minimum update time in milliseconds
       * \param alpha gain for running average filter
       */
      static sptr make(size_t itemsize, double update_rate_ms = 500.0, double alpha = 0.0001);

      virtual void set_alpha(double alpha) = 0;

      virtual double rate() = 0;

      virtual bool start() = 0;
      virtual bool stop() = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_PROBE_RATE_H */
