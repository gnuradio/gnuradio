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
#include <cstdio>

gri_fir_filter_with_buffer_ccf::gri_fir_filter_with_buffer_ccf(const std::vector<float> &taps)
{
  d_buffer = NULL;
  set_taps(taps);
}

gri_fir_filter_with_buffer_ccf::~gri_fir_filter_with_buffer_ccf()
{
  free(d_buffer);
}

gr_complex 
gri_fir_filter_with_buffer_ccf::filter (gr_complex input)
{
#if 0
  unsigned int i;
  
  for(i = ntaps()-1; i > 0; i--) {
    d_buffer[i] = d_buffer[i-1];
  }
  d_buffer[0] = input;

  gr_complex out = d_buffer[0]*d_taps[0];
  for(i = 1; i < ntaps(); i++) {
    out += d_buffer[i]*d_taps[i];
  }
  return out;

#else
  unsigned int i;

  d_buffer[d_idx] = input;
  d_buffer[d_idx+ntaps()] = input;
  //d_idx = (d_idx + 1) % ntaps();
  d_idx++;
  if(d_idx == ntaps())
    d_idx = 0;

  gr_complex out = d_buffer[d_idx]*d_taps[0];
  for(i = 1; i < ntaps(); i++) {
    out += d_buffer[d_idx + i]*d_taps[i];
  }
  return out;
#endif
}

void
gri_fir_filter_with_buffer_ccf::filterN (gr_complex output[],
					 const gr_complex input[],
					 unsigned long n)
{

}
