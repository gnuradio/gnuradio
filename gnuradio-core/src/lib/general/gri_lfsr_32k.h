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

#ifndef INCLUDED_GRI_LFSR_32k_H
#define INCLUDED_GRI_LFSR_32k_H

#include <gr_core_api.h>
#include <gri_lfsr_15_1_0.h>

/*!
 * \brief generate pseudo-random sequence of length 32768 bits.
 * \ingroup misc
 *
 * This is based on gri_lfsr_15_1_0 with an extra 0 added at the end
 * of the sequence.
 */

class GR_CORE_API gri_lfsr_32k {
  gri_lfsr_15_1_0	d_lfsr;
  unsigned int		d_count;

 public:
  gri_lfsr_32k () { reset (); }

  void reset (){
    d_lfsr.reset ();
    d_count = 0;
  }
    
  int next_bit (){
    if (d_count == 32767){
      d_count = 0;
      return 0;
    }
    d_count++;
    return d_lfsr.next_bit ();
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

  int next_short (){
    int	v = 0;
    for (int i = 0; i < 16; i++){
      v >>= 1;
      if (next_bit ())
	v |= 0x8000;
    }
    return v;
  }

};

#endif /* INCLUDED_GRI_LFSR_32k_H */
