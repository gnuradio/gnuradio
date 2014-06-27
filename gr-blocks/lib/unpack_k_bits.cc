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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/blocks/unpack_k_bits.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <iostream>

namespace gr {
  namespace blocks {
    namespace kernel {

      unpack_k_bits::unpack_k_bits(unsigned k)
        : d_k(k)
      {
        if(d_k == 0)
          throw std::out_of_range("unpack_k_bits: k must be > 0");
      }

      unpack_k_bits::~unpack_k_bits()
      {
      }

      void
      unpack_k_bits::unpack(unsigned char *bits, const unsigned char *bytes, int nbytes) const
      {
        int n = 0;
        for(int i = 0; i < nbytes; i++) {
          unsigned int t = bytes[i];
          for(int j = d_k - 1; j >= 0; j--)
            bits[n++] = (t >> j) & 0x01;
        }
      }

      void
      unpack_k_bits::unpack_rev(unsigned char *bits, const unsigned char *bytes, int nbytes) const
      {
        int n = 0;
        for(int i = 0; i < nbytes; i++) {
          unsigned int t = bytes[i];
          for(unsigned int j = 0; j < d_k; j++)
            bits[n++] = (t >> j) & 0x01;
        }
      }

      int
      unpack_k_bits::k() const
      {
        return d_k;
      }

    } /* namespace kernel */
  } /* namespace blocks */
} /* namespace gr */
