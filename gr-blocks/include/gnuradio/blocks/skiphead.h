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

#ifndef INCLUDED_GR_SKIPHEAD_H
#define INCLUDED_GR_SKIPHEAD_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <stddef.h>      // size_t

namespace gr {
  namespace blocks {

    /*!
     * \brief skips the first N items, from then on copies items to the output
     * \ingroup misc_blk
     *
     * \details
     * Useful for building test cases and sources which have metadata
     * or junk at the start
     */
    class BLOCKS_API skiphead : virtual public block
    {
    public:
      // gr::blocks::skiphead::sptr
      typedef boost::shared_ptr<skiphead> sptr;

      static sptr make(size_t itemsize,
                       uint64_t nitems_to_skip);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_SKIPHEAD_H */
