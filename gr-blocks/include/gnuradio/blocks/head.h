/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_HEAD_H
#define INCLUDED_GR_HEAD_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <stddef.h>			// size_t

namespace gr {
  namespace blocks {

    /*!
     * \brief copies the first N items to the output then signals done
     * \ingroup misc_blk
     *
     * \details
     * Useful for building test cases
     */
    class BLOCKS_API head : virtual public sync_block
    {
    public:
      // gr::blocks::head::sptr
      typedef boost::shared_ptr<head> sptr;

      static sptr make(size_t sizeof_stream_item,
                       uint64_t nitems);

      virtual void reset() = 0;
      virtual void set_length(uint64_t nitems) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_HEAD_H */
