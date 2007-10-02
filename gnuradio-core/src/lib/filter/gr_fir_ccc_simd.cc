/* -*- c++ -*- */
/*
 * Copyright 2002,2007 Free Software Foundation, Inc.
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
#include <config.h>
#endif
#include <gr_fir_ccc_simd.h>

#include <assert.h>
#include <malloc16.h>
#include <iostream>
#include <stdexcept>

using std::cerr;
using std::endl;

gr_fir_ccc_simd::gr_fir_ccc_simd ()
  : gr_fir_ccc_generic ()
{
  // cerr << "@@@ gr_fir_ccc_simd\n";

  d_ccomplex_dotprod = 0;
  
  d_aligned_taps[0] = 0;
  d_aligned_taps[1] = 0;
  d_aligned_taps[2] = 0;
  d_aligned_taps[3] = 0;
}

gr_fir_ccc_simd::gr_fir_ccc_simd (const std::vector<gr_complex> &new_taps)
  : gr_fir_ccc_generic (new_taps)
{
  // cerr << "@@@ gr_fir_ccc_simd\n";

  d_ccomplex_dotprod = 0;
  
  d_aligned_taps[0] = 0;
  d_aligned_taps[1] = 0;
  d_aligned_taps[2] = 0;
  d_aligned_taps[3] = 0;
  set_taps (new_taps);
}

gr_fir_ccc_simd::~gr_fir_ccc_simd ()
{
  free16Align (d_aligned_taps[0]);
  free16Align (d_aligned_taps[1]);
  free16Align (d_aligned_taps[2]);
  free16Align (d_aligned_taps[3]);
}

void
gr_fir_ccc_simd::set_taps (const std::vector<gr_complex> &inew_taps)
{
  gr_fir_ccc::set_taps (inew_taps);	// call superclass

  const std::vector<gr_complex> new_taps = gr_reverse(inew_taps);
  unsigned len = new_taps.size ();

  // Make 4 copies of the coefficients, one for each data alignment
  // Note use of special 16-byte-aligned version of calloc()
  
  for (unsigned i = 0; i < 4; i++){
    free16Align (d_aligned_taps[i]);	// free old value

    // this works because the bit representation of a IEEE floating point
    // +zero is all zeros.  If you're using a different representation,
    // you'll need to explictly set the result to the appropriate 0.0 value.
    
    d_aligned_taps[i] = (float *) calloc16Align (1 + (len + i - 1) / 2,
					       2 * 4 * sizeof (float));
    if (d_aligned_taps[i] == 0){
      // throw something...
      cerr << "@@@ gr_fir_ccc_simd d_aligned_taps[" << i << "] == 0\n";
    }

    for (unsigned j = 0; j < len; j++) {
      d_aligned_taps[i][2*(j+i)] = new_taps[j].real();
      d_aligned_taps[i][2*(j+i)+1] = new_taps[j].imag();
    }
  }
}

gr_complex 
gr_fir_ccc_simd::filter (const gr_complex input[])
{
  if (ntaps () == 0)
    return 0.0;

  if (((intptr_t) input & 0x7) != 0)
    throw std::invalid_argument("gr_complex must be 8-byte aligned");

  // Round input data address down to 16 byte boundary
  // NB: depending on the alignment of input[], memory
  // before input[] will be accessed. The contents don't matter since 
  // they'll be multiplied by zero coefficients. I can't conceive of any
  // situation where this could cause a segfault since memory protection
  // in the x86 machines is done on much larger boundaries.
  
  const gr_complex *ar = (gr_complex *)((unsigned long) input & ~15);

  // Choose one of 4 sets of pre-shifted coefficients. al is both the
  // index into d_aligned_taps[] and the number of 0 words padded onto
  // that coefficients array for alignment purposes.

  unsigned al = input - ar;

  // call assembler routine to do the work, passing number of 2x4-float blocks.

  // assert (((unsigned long) ar & 15) == 0);
  // assert (((unsigned long) d_aligned_taps[al] & 15) == 0);

  // cerr << "ar: " << ar << " d_aligned_taps[ar]: " << d_aligned_taps[al]
  //  << " (ntaps() + al - 1)/2 + 1: " << (ntaps() + al -1) / 2 + 1 << endl;

  float result[2];

  d_ccomplex_dotprod ((float*)ar, d_aligned_taps[al], (ntaps() + al - 1) / 2 + 1, result);

  // cerr << "result = " << result[0] << " " << result[1] << endl;

  return gr_complex(result[0], result[1]);
}
