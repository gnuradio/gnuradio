/* -*- c++ -*- */
/*
 * Copyright 2006,2012,2017 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "bch3121.h"

namespace gr {
  namespace pager {

    int bch_n = 31, bch_k=21;
    // The generating polynomial is:
    // g(x) = x^10 + x^9 + x^8 + x^6 + x^5 + x^3 + 1
    uint32_t bch_polynomial = 0x769;


    static inline uint8_t
    even_parity(uint32_t x)
    {
    	x ^= x >> 16;
    	x ^= x >> 8;
    	x ^= x >> 4;
    	x &= 0xf;
	// 0x6996 is 0b110100110010110, vector of parities for x
    	return (0x6996 >> x) & 1;
    }

    static uint32_t syndrome(uint32_t data)
    {
      // Shift out the parity bit
      uint32_t syndrome = data >> 1;
      // Set up our initial bitmask and polynomial coefficient for
      // sliding term-wise calculation
      uint32_t mask = 1L << (bch_n-1);
      uint32_t coefficient = bch_polynomial << (bch_k-1);

      //Term-wise in codeword polynomial
      int n;
      for(n = bch_k; n > 0; mask >>= 1, coefficient >>= 1, n--)
	if (syndrome & mask)
	  syndrome ^= coefficient;

    	if (even_parity(data))
	  syndrome |= (1 << (bch_n - bch_k));

    	return syndrome;
    }

    // Corrects supplied data word according to BCH3121 encoding and
    // returns the number of errors detected/corrected.  For now, brute
    // force it, until there's time to implement Berlekamp's algorithm


    uint32_t bch3121(uint32_t &data)
    {
      uint32_t t;
      int i, j;

      if (!syndrome(data))
        return 0;

      for (i=0; i<32; i++) {
	t = data ^ (1<<i);
	if (!syndrome(t)) {
	  data = t;
          return 1;

        }
      }

      for (i=0; i<32; i++) {
	for (j=i+1; j<32; j++) {
	  t = data ^ ((1<<i) | (1<<j));
	  if (!syndrome(t)){
            data = t;
            return 2;
          }
	}
      }
      // Error condition
      return -1;
    }
    
  } /* namespace pager */
} /* namespace gr */
