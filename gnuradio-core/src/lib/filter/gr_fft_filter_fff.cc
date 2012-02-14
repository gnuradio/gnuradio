/* -*- c++ -*- */
/*
 * Copyright 2005,2010 Free Software Foundation, Inc.
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

#include <gr_fft_filter_fff.h>
#include <gri_fft_filter_fff_generic.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <stdexcept>

#include <cstdio>
#include <iostream>
#include <string.h>

gr_fft_filter_fff_sptr gr_make_fft_filter_fff (int decimation,
					       const std::vector<float> &taps,
					       int nthreads)
{
  return gnuradio::get_initial_sptr(new gr_fft_filter_fff (decimation, taps, nthreads));
}


gr_fft_filter_fff::gr_fft_filter_fff (int decimation,
				      const std::vector<float> &taps,
				      int nthreads)
  : gr_sync_decimator ("fft_filter_fff",
		       gr_make_io_signature (1, 1, sizeof (float)),
		       gr_make_io_signature (1, 1, sizeof (float)),
		       decimation),
    d_updated(false)
{
  set_history(1);
  
#if 1 // don't enable the sse version until handling it is worked out
  d_filter = new gri_fft_filter_fff_generic(decimation, taps, nthreads);
#else
    d_filter = new gri_fft_filter_fff_sse(decimation, taps);
#endif

  d_new_taps = taps;
  d_nsamples = d_filter->set_taps(taps);
  set_output_multiple(d_nsamples);
}

gr_fft_filter_fff::~gr_fft_filter_fff ()
{
  delete d_filter;
}

void
gr_fft_filter_fff::set_taps (const std::vector<float> &taps)
{
  d_new_taps = taps;
  d_updated = true;
}

std::vector<float>
gr_fft_filter_fff::taps () const
{
  return d_new_taps;
}

void
gr_fft_filter_fff::set_nthreads(int n)
{
  if(d_filter)
    d_filter->set_nthreads(n);
}

int
gr_fft_filter_fff::nthreads() const
{
  if(d_filter)
    return d_filter->nthreads();
  else
    return 0;
}

int
gr_fft_filter_fff::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];

  if (d_updated){
    d_nsamples = d_filter->set_taps(d_new_taps);
    d_updated = false;
    set_output_multiple(d_nsamples);
    return 0;				// output multiple may have changed
  }

  assert(noutput_items % d_nsamples == 0);
  
  d_filter->filter(noutput_items, in, out);

  //assert((out - (float *) output_items[0]) == noutput_items);

  return noutput_items;
}
