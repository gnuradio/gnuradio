/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gr_fir_fff_altivec.h>
#include <stdexcept>
#include <assert.h>
#include <gr_math.h>
#include <gr_altivec.h>
#include <dotprod_fff_altivec.h>
#include <string.h>
#include "posix_memalign.h"

gr_fir_fff_altivec::gr_fir_fff_altivec()
  : gr_fir_fff_generic(),
    d_naligned_taps(0), d_aligned_taps(0)
{
}

gr_fir_fff_altivec::gr_fir_fff_altivec (const std::vector<float> &new_taps)
  : gr_fir_fff_generic(new_taps),
    d_naligned_taps(0), d_aligned_taps(0)
{
  set_taps(new_taps);
}

gr_fir_fff_altivec::~gr_fir_fff_altivec()
{
  if (d_aligned_taps){
    free(d_aligned_taps);
    d_aligned_taps = 0;
  }
}

void
gr_fir_fff_altivec::set_taps(const std::vector<float> &inew_taps)
{
  gr_fir_fff_generic::set_taps(inew_taps);	// call superclass
  d_naligned_taps = gr_p2_round_up(ntaps(), FLOATS_PER_VEC);

  if (d_aligned_taps){
    free(d_aligned_taps);
    d_aligned_taps = 0;
  }
  void *p;
  int r = posix_memalign(&p,  sizeof(vec_float4), d_naligned_taps * sizeof(d_aligned_taps[0]));
  if (r != 0){
    throw std::bad_alloc();
  }
  d_aligned_taps = (float *) p;
  memcpy(d_aligned_taps, &d_taps[0], ntaps() * sizeof(d_aligned_taps[0]));
  for (size_t i = ntaps(); i < d_naligned_taps; i++)
    d_aligned_taps[i] = 0.0;
}


float 
gr_fir_fff_altivec::filter (const float input[])
{
  if (d_naligned_taps == 0)
    return 0.0;
  
  return dotprod_fff_altivec(input, d_aligned_taps, d_naligned_taps);
}
