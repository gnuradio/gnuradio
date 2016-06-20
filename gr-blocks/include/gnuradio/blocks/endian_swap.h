/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_ENDIAN_SWAP_H
#define INCLUDED_GR_ENDIAN_SWAP_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Convert stream of items into their byte swapped version
     * \ingroup stream_operators_blk
     */
    class BLOCKS_API endian_swap : virtual public sync_block
    {
    public:
      // gr::blocks::endian_swap::sptr
      typedef boost::shared_ptr<endian_swap> sptr;

      /*!
       * Make an endian swap block.
       *
       * \param item_size_bytes number of bytes per item, 1=no-op,
       *        2=uint16_t, 4=uint32_t, 8=uint64_t
       */
      static sptr make(size_t item_size_bytes=1);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ENDIAN_SWAP_H */
