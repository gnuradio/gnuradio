/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GRI_LFSR_15_1_0_H
#define INCLUDED_GRI_LFSR_15_1_0_H

#include <gr_core_api.h>

/*!
 * \brief Linear Feedback Shift Register using primitive polynomial x^15 + x + 1
 * \ingroup misc
 *
 * Generates a maximal length pseudo-random sequence of length 2^15 - 1 bits.
 */

class GR_CORE_API gri_lfsr_15_1_0 {
  unsigned long	d_sr;		// shift register

 public:

  gri_lfsr_15_1_0 () { reset (); }

  void reset () { d_sr = 0x7fff; }
  
  int next_bit (){
    d_sr = ((((d_sr >> 1) ^ d_sr) & 0x1) << 14) | (d_sr >> 1);
    return d_sr & 0x1;
  }

  int next_byte (){
    int	v = 0;
    for (int i = 0; i < 8; i++){
      v >>= 1;
      if (next_bit ())
	v |= 0x80;
    }
    return v;
  }
};

#endif /* INCLUDED_GRI_LFSR_15_1_0_H */