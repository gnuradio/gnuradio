/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <gri_fir_filter_with_buffer_ccf.h>

gri_fir_filter_with_buffer_ccf::gri_fir_filter_with_buffer_ccf(const std::vector<float> &taps)
{
  d_buffer = NULL;
  set_taps(taps);
}

gri_fir_filter_with_buffer_ccf::~gri_fir_filter_with_buffer_ccf()
{
  if(d_buffer != NULL)
    free(d_buffer);
}

void
gri_fir_filter_with_buffer_ccf::set_taps (const std::vector<float> &taps)
{
  d_taps = gr_reverse(taps);
  
  if(d_buffer != NULL) {
    free(d_buffer);
    d_buffer = NULL;
  }
  
  // FIXME: memalign this to 16-byte boundaries for SIMD later
  size_t t = sizeof(gr_complex) * 2 * d_taps.size();
  d_buffer = (gr_complex*)malloc(t);
  memset(d_buffer, 0x00, t);
  d_idx = 0;
}

gr_complex
gri_fir_filter_with_buffer_ccf::filter (gr_complex input)
{
  unsigned int i;

  d_buffer[d_idx] = input;
  d_buffer[d_idx+ntaps()] = input;

  // using the later for the case when ntaps=0;
  // profiling shows this doesn't make a difference
  //d_idx = (d_idx + 1) % ntaps();
  d_idx++;
  if(d_idx >= ntaps())
    d_idx = 0;

  gr_complex out = 0;
  for(i = 0; i < ntaps(); i++) {
    out +=  d_buffer[d_idx + i] * d_taps[i];
  }
  return (gr_complex)out;
}

gr_complex
gri_fir_filter_with_buffer_ccf::filter (const gr_complex input[], unsigned long dec)
{
  unsigned int i;

  for(i = 0; i < dec; i++) {
    d_buffer[d_idx] = input[i];
    d_buffer[d_idx+ntaps()] = input[i];
    d_idx++;
    if(d_idx >= ntaps())
      d_idx = 0;
  }

  gr_complex out = 0;
  for(i = 0; i < ntaps(); i++) {
    out +=  d_buffer[d_idx + i] * d_taps[i];
  }
  return (gr_complex)out;
}

void
gri_fir_filter_with_buffer_ccf::filterN (gr_complex output[],
		 const gr_complex input[],
		 unsigned long n)
{
  for(unsigned long i = 0; i < n; i++) {
    output[i] = filter(input[i]);
  }
}

void
gri_fir_filter_with_buffer_ccf::filterNdec (gr_complex output[],
		    const gr_complex input[],
		    unsigned long n,
		    unsigned long decimate)
{
  unsigned long j = 0;
  for(unsigned long i = 0; i < n; i++) {
    output[i] = filter(&input[j], decimate);
    j += decimate;
  }
}
