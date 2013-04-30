/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#ifndef _ATSC_SLIDING_CORRELATOR_H_
#define _ATSC_SLIDING_CORRELATOR_H_

#include <gnuradio/atsc/api.h>
#include <string.h>

#include <gnuradio/atsc/pnXXX_impl.h>
//extern const unsigned char atsc_pn511[511];
//extern const unsigned char atsc_pn63[63];

/*!
 * \brief look for the PN 511 field sync pattern
 */
class ATSC_API atsci_sliding_correlator {
 public:

  atsci_sliding_correlator ();
  ~atsci_sliding_correlator (){};

  //! input hard decision bit, return correlation (0,511)
  // Result is the number of wrong bits.
  // E.g., 0 -> perfect match; 511 -> all bits are wrong

  int input_bit (int bit);

  //! input sample, return correlation (0,511)
  // Result is the number of wrong bits.
  // E.g., 0 -> perfect match; 511 -> all bits are wrong

  int input_int (int sample){
    return input_bit (sample < 0 ? 0 : 1);
  }

  //! input sample, return correlation (0,511)
  // Result is the number of wrong bits.
  // E.g., 0 -> perfect match; 511 -> all bits are wrong

  int input_float (float sample){
    return input_bit (sample < 0 ? 0 : 1);
  }

  void reset () { input.reset (); }

 private:

  typedef unsigned long	srblock;
  static const int bits_per_char = 8;
  static const int srblock_bitsize = sizeof (srblock) * bits_per_char;
  static const int NSRBLOCKS = (511 + srblock_bitsize - 1) / srblock_bitsize;

  class shift_reg {
  public:
    shift_reg ()  { reset (); }
    void reset () { memset (d, 0, sizeof (d)); }
    void shift_in (int bit);
    srblock	d[NSRBLOCKS];
  };

  shift_reg	mask;		// pattern we're looking for
  shift_reg	input;		// current input window
  shift_reg	and_mask;	// bits to consider
};

#endif /* _ATSC_SLIDING_CORRELATOR_H_ */
