/* -*- c++ -*- */
/*
 * Copyright 2002,2006 Free Software Foundation, Inc.
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

#include <gnuradio/atsc/sliding_correlator_impl.h>
#include <gnuradio/atsc/pnXXX_impl.h>

// #define TRY_BACKWARDS

/*
 * Return the number of 1's in v.
 * This magic code is explained wonderfully in the AMD Athlon
 * optimization guide, pg 136.
 */
static inline int popcount32 (unsigned long v)
{
  unsigned long w = v - ((v >> 1) & 0x55555555);
  unsigned long x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
  unsigned long y = (x + (x >> 4)) & 0x0f0f0f0f;
  unsigned long z = (y * 0x01010101) >> 24;

  return z;
}

atsci_sliding_correlator::atsci_sliding_correlator ()
{
#if defined(TRY_BACKWARDS)
  for (int i = 511 - 1; i >= 0; i--)
    mask.shift_in (atsc_pn511[i]);
#else
  for (unsigned i = 0; i < 511; i++)
    mask.shift_in (atsc_pn511[i]);
#endif

  and_mask.shift_in (0);
  for (int i = 0; i < 511; i++)
    and_mask.shift_in (1);
}

/*
 * we shift in from the top of the register towards the bottom
 *
 * +-- bits enter here...
 * |
 * v
 * block 0 | block 1 | block 2 ... | block N-1
 *					     |
 *					     +--> ... and drop out here
 *
 */
inline void
atsci_sliding_correlator::shift_reg::shift_in (int bit)
{
  for (int i = NSRBLOCKS - 1; i > 0; i--)
    d[i] = ((d[i-1] & 0x1) << (srblock_bitsize - 1)) | (d[i] >> 1);

  d[0] = (((srblock) bit) << (srblock_bitsize - 1)) | (d[0] >> 1);
}

int
atsci_sliding_correlator::input_bit (int bit)
{
  input.shift_in (bit);

  // can probably get a win by unrolling the loop, if we need to.
  int	count = 0;
  for (int i = 0; i < NSRBLOCKS; i++)
    count += popcount32 ((input.d[i] ^ mask.d[i]) & and_mask.d[i]);

  return count;
}

