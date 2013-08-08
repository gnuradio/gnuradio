/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_NULL_SINK_H
#define INCLUDED_GR_NULL_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <stddef.h>			// size_t

namespace gr {
  namespace blocks {

    /*!
     * \brief Bit bucket. Use as a termination point when a sink is
     * required and we don't want to do anything real.
     * \ingroup misc_blk
     */
    class BLOCKS_API null_sink : virtual public sync_block
    {
    public:
      // gr::blocks::null_sink::sptr
      typedef boost::shared_ptr<null_sink> sptr;

      /*!
       * Build a null sink block.
       *
       * \param sizeof_stream_item size of the stream items in bytes.
       */
      static sptr make(size_t sizeof_stream_item);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_NULL_SINK_H */
