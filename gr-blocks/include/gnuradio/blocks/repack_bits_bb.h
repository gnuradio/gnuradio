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
#include <gnuradio/endianness.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Repack \p k bits from the input stream onto \p l bits of the output stream.
     * \ingroup byte_operators_blk
     *
     * \details
     * No bits are lost here; any value for k and l (within [1, 8]) is allowed.
     * On every fresh input byte, it starts reading on the LSB, and starts copying
     * to the LSB as well.
     *
     * When supplying a tag name, this block operates on tagged streams.
     * In this case, it can happen that the input data or the output data
     * becomes unaligned when k * input length is not equal to l * output length.
     * In this case, the \p align_output parameter is used to decide which
     * data packet to align.
     *
     * Usually, \p align_output is false for unpacking (k=8, l < 8) and false for
     * reversing that.
     *
     * \section gr_blocks_repack_example Example
     *
     * Say you're tx'ing 8-PSK and therefore set k=8, l=3 on the transmit side
     * before the modulator. Now assume you're transmitting a single byte of data.
     * Your incoming tagged stream has length 1, the outgoing has length 3. However,
     * the third item is actually only carrying 2 bits of relevant data, the bits
     * do not align with the boundaries. So you set \p align_output = false,
     * because the output can be unaligned.
     *
     * Now say you're doing the inverse: packing those three items into full
     * bytes. How do you interpret those three bytes? Without this flag,
     * you'd have to assume there's 9 relevant bits in there, so you'd end up
     * with 2 bytes of output data. But in the packing case, you want the
     * \b output to be aligned; all output bits must be useful. By asserting this flag,
     * the packing algorithm tries to do this and in this case assumes that
     * since we have alignment after 8 bits, the 9th can be discarded.
     */
    class BLOCKS_API repack_bits_bb : virtual public tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<repack_bits_bb> sptr;

      /*!
       * \param k Number of relevant bits on the input stream
       * \param l Number of relevant bits on the output stream
       * \param tsb_tag_key If not empty, this is the key for the length tag.
       * \param align_output If tsb_tag_key is given, this controls if the input
       *                     or the output is aligned.
       * \param endianness The endianness of the output data stream (LSB or MSB).
       */
      static sptr make(int k, int l=8, const std::string &tsb_tag_key="",
                       bool align_output=false, endianness_t endianness=GR_LSB_FIRST);
      virtual void set_k_and_l(int k, int l) =0;//callback function for bits per input byte k and bits per output byte l.
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_REPACK_BITS_BB_H */
