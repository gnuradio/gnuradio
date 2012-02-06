/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#include <gr_fft_vfc.h>
#include <gr_io_signature.h>
#include <gri_fft.h>
#include <math.h>
#include <stdexcept>
#include <string.h>
#include <cstdio>


// FIXME after this is working, change to use native real to complex fft.
// It should run twice as fast.




gr_fft_vfc_sptr
gr_make_fft_vfc (int fft_size, bool forward,
		 const std::vector<float> &window,
		 int nthreads)
{
  return gnuradio::get_initial_sptr(new gr_fft_vfc (fft_size, forward,
						    window, nthreads));
}

gr_fft_vfc::gr_fft_vfc (int fft_size, bool forward,
			const std::vector<float> &window,
			int nthreads)
  : gr_sync_block ("fft_vfc",
		   gr_make_io_signature (1, 1, fft_size * sizeof (float)),
		   gr_make_io_signature (1, 1, fft_size * sizeof (gr_complex))),
    d_fft_size(fft_size), d_window()
{
  if (!forward){
    fprintf (stderr, "fft_vfc: forward must == true\n");
    throw std::invalid_argument ("fft_vfc: forward must == true");
  }

  d_fft = new gri_fft_complex (d_fft_size, forward, nthreads);

  set_window(window);
}

gr_fft_vfc::~gr_fft_vfc ()
{
  delete d_fft;
}

void
gr_fft_vfc::set_nthreads(int n)
{
  d_fft->set_nthreads(n);
}

int
gr_fft_vfc::nthreads() const
{
  return d_fft->nthreads();
}

int
gr_fft_vfc::work (int noutput_items,
		  gr_vector_const_void_star &input_items,
		  gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  unsigned int output_data_size = output_signature()->sizeof_stream_item (0);

  int count = 0;

  while (count++ < noutput_items){

    // copy input into optimally aligned buffer

    if (d_window.size()){
      gr_complex *dst = d_fft->get_inbuf();
      for (unsigned int i = 0; i < d_fft_size; i++)		// apply window
	dst[i] = in[i] * d_window[i];
    }
    else {
      gr_complex *dst = d_fft->get_inbuf();
      for (unsigned int i = 0; i < d_fft_size; i++)		// float to complex conversion
	dst[i] = in[i];				       
    }

    // compute the fft
    d_fft->execute ();

    // cpoy result to our output
    memcpy (out, d_fft->get_outbuf (), output_data_size);

    in  += d_fft_size;
    out += d_fft_size;
  }

  return noutput_items;
}

bool 
gr_fft_vfc::set_window(const std::vector<float> &window)
{
  if(window.size()==0 || window.size()==d_fft_size) {
    d_window=window;
    return true;
  }
  else 
    return false;
}
