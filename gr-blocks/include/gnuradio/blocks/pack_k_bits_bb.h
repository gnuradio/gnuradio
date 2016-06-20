/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PACK_K_BITS_BB_H
#define	INCLUDED_GR_PACK_K_BITS_BB_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Converts a stream of bytes with 1 bit in the LSB to a
     * byte with k relevant bits.
     *
     * This block takes in K bytes at a time, and uses the least significant
     * bit to form a new byte.
     *
     * Example:
     * k = 4
     * in = [0,1,0,1, 0x81,0x00,0x00,0x00]
     * out = [0x05, 0x08]
     *
     * \ingroup byte_operators_blk
     */
    class BLOCKS_API pack_k_bits_bb : virtual public sync_decimator
    {
    public:
      // gr::blocks::pack_k_bits_bb::sptr
      typedef boost::shared_ptr<pack_k_bits_bb> sptr;

      /*!
       * \brief Make a pack_k_bits block.
       * \param k number of bits to be packed.
       */
      static sptr make(unsigned k);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_PACK_K_BITS_BB_H */
