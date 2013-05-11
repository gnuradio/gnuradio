/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_REPACK_BITS_BB_H
#define INCLUDED_BLOCKS_REPACK_BITS_BB_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Pack \p k bits from the input stream onto \p l bits of the output stream.
     * \ingroup byte_operators_blk
     *
     * \details
     * No bits are lost here; any value for k and l (within [1, 8]) is allowed.
     * On every fresh input byte, it starts reading on the LSB, and starts copying
     * to the LSB as well.
     *
     * If a packet length tag is given, this block assumes a tagged stream.
     * In this case, the tag with the packet length is updated accordingly.
     * Also, the number of input bits is padded with zeros if the number of input
     * bits is not an integer multiple of \p l, or bits are truncated from the input
     * if \p align_output is set to true.
     */
    class BLOCKS_API repack_bits_bb : virtual public tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<repack_bits_bb> sptr;

      /*!
       * \param k Number of relevant bits on the input stream
       * \param l Number of relevant bits on the output stream
       * \param len_tag_key If not empty, this is the key for the length tag.
       * \param align_output If len_tag_key is given, this controls if the input
       *                     or the output is aligned.
       */
      static sptr make(int k, int l=8, const std::string &len_tag_key="", bool align_output=false);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_REPACK_BITS_BB_H */

