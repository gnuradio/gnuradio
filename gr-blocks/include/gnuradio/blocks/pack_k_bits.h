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

#ifndef INCLUDED_GR_BLOCKS_PACK_K_BITS_H
#define	INCLUDED_GR_BLOCKS_PACK_K_BITS_H

#include <gnuradio/blocks/api.h>
#include <vector>

namespace gr {
  namespace blocks {
    namespace kernel {

      /*!
       * \brief Converts a vector of bytes with 1 bit in the LSB to a
       * byte with k relevant bits.
       *
       * Example:
       * k = 4
       * in = [0,1,0,1, 0x81,0x00,0x00,0x00]
       * out = [0x05, 0x08]
       *
       * k = 8
       * in = [1,1,1,1, 0,1,0,1, 0,0,0,0, 1,0,0,0]
       * out = [0xf5, 0x08]
       * \ingroup byte_operators_blk
       */
      class BLOCKS_API pack_k_bits
      {
      public:
        /*!
         * \brief Make a pack_k_bits object.
         * \param k number of bits to be packed.
         */
        pack_k_bits(unsigned k);
        ~pack_k_bits();

        /*!
         * \brief Perform the packing.
         *
         * This block performs no bounds checking. It assumes that the
         * input, \p in, has of length k*nbytes and that the output
         * vector, \p out, has \p nbytes available for writing.
         *
         * \param bytes output vector (k-bits per byte) of the unpacked data
         * \param bits The input vector of bits to pack
         * \param nbytes The number of output bytes
         */
        void pack(unsigned char *bytes, const unsigned char *bits, int nbytes) const;

        /*!
         * Same as pack() but reverses the bits.
         */
        void pack_rev(unsigned char *bytes, const unsigned char *bits, int nbytes) const;

        int k() const;

      private:
        unsigned d_k;
      };

    } /* namespace kernel */
  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_BLOCKS_PACK_K_BITS_H */
