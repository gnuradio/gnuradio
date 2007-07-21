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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gr_fir_fsf_simd.h>

#include <assert.h>
#include <malloc16.h>
#include <iostream>

using std::cerr;

gr_fir_fsf_simd::gr_fir_fsf_simd ()
  : gr_fir_fsf_generic ()
{
  // cerr << "@@@ gr_fir_fsf_simd\n";

  d_float_dotprod = 0;
  
  d_aligned_taps[0] = 0;
  d_aligned_taps[1] = 0;
  d_aligned_taps[2] = 0;
  d_aligned_taps[3] = 0;
}

gr_fir_fsf_simd::gr_fir_fsf_simd (const std::vector<float> &new_taps)
  : gr_fir_fsf_generic (new_taps)
{
  // cerr << "@@@ gr_fir_fsf_simd\n";

  d_float_dotprod = 0;
  
  d_aligned_taps[0] = 0;
  d_aligned_taps[1] = 0;
  d_aligned_taps[2] = 0;
  d_aligned_taps[3] = 0;
  set_taps (new_taps);
}

gr_fir_fsf_simd::~gr_fir_fsf_simd ()
{
  free16Align (d_aligned_taps[0]);
  free16Align (d_aligned_taps[1]);
  free16Align (d_aligned_taps[2]);
  free16Align (d_aligned_taps[3]);
}

void
gr_fir_fsf_simd::set_taps (const std::vector<float> &inew_taps)
{
  gr_fir_fsf::set_taps (inew_taps);	// call superclass
  const std::vector<float> new_taps = gr_reverse(inew_taps);
  unsigned len = new_taps.size ();

  // Make 4 copies of the coefficients, one for each data alignment
  // Note use of special 16-byte-aligned version of calloc()
  
  for (unsigned i = 0; i < 4; i++){
    free16Align (d_aligned_taps[i]);	// free old value

    // this works because the bit representation of a IEEE floating point
    // +zero is all zeros.  If you're using a different representation,
    // you'll need to explictly set the result to the appropriate 0.0 value.
    
    d_aligned_taps[i] = (float *) calloc16Align (1 + (len + i - 1) / 4,
					       4 * sizeof (float));
    if (d_aligned_taps[i] == 0){
      // throw something...
      cerr << "@@@ gr_fir_fsf_simd d_aligned_taps[" << i << "] == 0\n";
    }

    for (unsigned j = 0; j < len; j++)
      d_aligned_taps[i][j+i] = new_taps[j];
  }
}

short 
gr_fir_fsf_simd::filter (const float input[])
{
  if (ntaps () == 0)
    return 0;


  // Round input data address down to 16 byte boundary
  // NB: depending on the alignment of input[], memory
  // before input[] will be accessed. The contents don't matter since 
  // they'll be multiplied by zero coefficients. I can't conceive of any
  // situation where this could cause a segfault since memory protection
  // in the x86 machines is done on much larger boundaries.
  
  const float *ar = (float *)((unsigned long) input & ~15);

  // Choose one of 4 sets of pre-shifted coefficients. al is both the
  // index into d_aligned_taps[] and the number of 0 words padded onto
  // that coefficients array for alignment purposes.

  unsigned al = input - ar;

  // call assembler routine to do the work, passing number of 4-float blocks.

  // assert (((unsigned long) ar & 15) == 0);
  // assert (((unsigned long) d_aligned_taps[al] & 15) == 0);

  // cerr << "ar: " << ar << " d_aligned_taps[ar]: " << d_aligned_taps[al]
  // << " (ntaps() + al - 1)/4 + 1: " << (ntaps() + al -1) / 4 + 1 << endl;
  
  float r = d_float_dotprod (ar, d_aligned_taps[al], (ntaps() + al - 1) / 4 + 1);

  // cerr << "result = " << r << endl;

  return (short) r;	// FIXME? may want to saturate here
}
