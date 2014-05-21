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

#include <gnuradio/blocks/pack_k_bits.h>
#include <stdexcept>
#include <iostream>

namespace gr {
  namespace blocks {
    namespace kernel {

      pack_k_bits::pack_k_bits(unsigned k)
        : d_k(k)
      {
        if(d_k == 0)
          throw std::out_of_range("pack_k_bits: k must be > 0");
      }

      pack_k_bits::~pack_k_bits()
      {
      }

      void
      pack_k_bits::pack(unsigned char *bytes, const unsigned char *bits, int nbytes) const
      {
        for(int i = 0; i < nbytes; i++) {
          bytes[i] = 0x00;
          for(unsigned int j = 0; j < d_k; j++) {
            bytes[i] |= (0x01 & bits[i*d_k+j])<<(d_k-j-1);
          }
        }
      }

      void
      pack_k_bits::pack_rev(unsigned char *bytes, const unsigned char *bits, int nbytes) const
      {
        for(int i = 0; i < nbytes; i++) {
          bytes[i] = 0x00;
          for(unsigned int j = 0; j < d_k; j++) {
            bytes[i] |= (0x01 & bits[i*d_k+j])<<j;
          }
        }
      }

    } /* namespace kernel */
  } /* namespace blocks */
} /* namespace gr */
