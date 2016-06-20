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

#ifndef INCLUDED_GR_BLOCKS_UNPACK_K_BITS_H
#define	INCLUDED_GR_BLOCKS_UNPACK_K_BITS_H

#include <gnuradio/blocks/api.h>
#include <vector>

namespace gr {
  namespace blocks {
    namespace kernel {

      /*!
       * \brief Converts a byte with k relevant bits to k output bytes with 1 bit in the LSB.
       *
       * This is the algorithm kernel for the gr::blocks::unpack_k_bits_bb block.
       *
       * Example:
       * k = 4
       * in = [0xf5, 0x08]
       * out = [0,1,0,1, 1,0,0,0]
       *
       * k = 8
       * in = [0xf5, 0x08]
       * out = [1,1,1,1, 0,1,0,1, 0,0,0,0, 1,0,0,0]
       * \ingroup byte_operators_blk
       */
      class BLOCKS_API unpack_k_bits
      {
      public:
        /*!
         * \brief Make an unpack_k_bits object.
         * \param k number of bits to unpack.
         */
        unpack_k_bits(unsigned k);
        ~unpack_k_bits();

        /*!
         * \brief Perform the unpacking.
         *
         * This function performs no bounds checking. It assumes that the
         * input, \p in, has of length \p nbytes and that the output
         * vector, \p out, has k*nbytes available for writing.
         *
         * \param bits output vector (1-bit per byte) of the unpacked data
         * \param bytes The input vector of bytes to unpack
         * \param nbytes The number of input bytes
         */
        void unpack(unsigned char *bits, const unsigned char *bytes, int nbytes) const;

        /*!
         * Unpacks in reverse order from unpack().
         */
        void unpack_rev(unsigned char *bits, const unsigned char *bytes, int nbytes) const;

        int k() const;

      private:
        unsigned d_k;
      };

    } /* namespace kernel */
  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_BLOCKS_UNPACK_K_BITS_BB_H */
