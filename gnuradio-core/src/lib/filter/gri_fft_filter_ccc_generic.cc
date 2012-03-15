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
#include "config.h"
#endif

#include <gri_fft_filter_ccc_generic.h>
#include <gri_fft.h>
#include <volk/volk.h>
#include <assert.h>
#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <fftw3.h>

gri_fft_filter_ccc_generic::gri_fft_filter_ccc_generic (int decimation, 
							const std::vector<gr_complex> &taps,
							int nthreads)
  : d_fftsize(-1), d_decimation(decimation), d_fwdfft(0), d_invfft(0), d_nthreads(nthreads)
{
  set_taps(taps);
}

gri_fft_filter_ccc_generic::~gri_fft_filter_ccc_generic ()
{
  delete d_fwdfft;
  delete d_invfft;
  gri_fft_free(d_xformed_taps);
}

#if 0
static void 
print_vector_complex(const std::string label, const std::vector<gr_complex> &x)
{
  std::cout << label;
  for (unsigned i = 0; i < x.size(); i++)
    std::cout << x[i] << " ";
  std::cout << "\n";
}
#endif


/*
 * determines d_ntaps, d_nsamples, d_fftsize, d_xformed_taps
 */
int
gri_fft_filter_ccc_generic::set_taps (const std::vector<gr_complex> &taps)
{
  int i = 0;
  compute_sizes(taps.size());

  d_tail.resize(tailsize());
  for (i = 0; i < tailsize(); i++)
    d_tail[i] = 0;

  gr_complex *in = d_fwdfft->get_inbuf();
  gr_complex *out = d_fwdfft->get_outbuf();

  float scale = 1.0 / d_fftsize;
  
  // Compute forward xform of taps.
  // Copy taps into first ntaps slots, then pad with zeros
  for (i = 0; i < d_ntaps; i++)
    in[i] = taps[i] * scale;

  for (; i < d_fftsize; i++)
    in[i] = 0;

  d_fwdfft->execute();		// do the xform

  // now copy output to d_xformed_taps
  for (i = 0; i < d_fftsize; i++)
    d_xformed_taps[i] = out[i];
  
  return d_nsamples;
}

// determine and set d_ntaps, d_nsamples, d_fftsize

void
gri_fft_filter_ccc_generic::compute_sizes(int ntaps)
{
  int old_fftsize = d_fftsize;
  d_ntaps = ntaps;
  d_fftsize = (int) (2 * pow(2.0, ceil(log(double(ntaps)) / log(2.0))));
  d_nsamples = d_fftsize - d_ntaps + 1;

  if (0)
    fprintf(stderr, "gri_fft_filter_ccc_generic: ntaps = %d, fftsize = %d, nsamples = %d\n",
	    d_ntaps, d_fftsize, d_nsamples);

  assert(d_fftsize == d_ntaps + d_nsamples -1 );

  if (d_fftsize != old_fftsize){	// compute new plans
    delete d_fwdfft;
    delete d_invfft;
    d_fwdfft = new gri_fft_complex(d_fftsize, true, d_nthreads);
    d_invfft = new gri_fft_complex(d_fftsize, false, d_nthreads);
    d_xformed_taps = gri_fft_malloc_complex(d_fftsize);
  }
}

void
gri_fft_filter_ccc_generic::set_nthreads(int n)
{
  d_nthreads = n;
  if(d_fwdfft)
    d_fwdfft->set_nthreads(n);
  if(d_invfft)
    d_invfft->set_nthreads(n);
}

int
gri_fft_filter_ccc_generic::nthreads() const
{
  return d_nthreads;
}

int
gri_fft_filter_ccc_generic::filter (int nitems, const gr_complex *input, gr_complex *output)
{
  int dec_ctr = 0;
  int j = 0;
  int ninput_items = nitems * d_decimation;

  for (int i = 0; i < ninput_items; i += d_nsamples){
    
    memcpy(d_fwdfft->get_inbuf(), &input[i], d_nsamples * sizeof(gr_complex));

    for (j = d_nsamples; j < d_fftsize; j++)
      d_fwdfft->get_inbuf()[j] = 0;

    d_fwdfft->execute();	// compute fwd xform
    
    gr_complex *a = d_fwdfft->get_outbuf();
    gr_complex *b = d_xformed_taps;
    gr_complex *c = d_invfft->get_inbuf();

    volk_32fc_x2_multiply_32fc_a(c, a, b, d_fftsize);
    
    d_invfft->execute();	// compute inv xform

    // add in the overlapping tail

    for (j = 0; j < tailsize(); j++)
      d_invfft->get_outbuf()[j] += d_tail[j];

    // copy nsamples to output
    j = dec_ctr;
    while (j < d_nsamples) {
      *output++ = d_invfft->get_outbuf()[j];
      j += d_decimation;
    }
    dec_ctr = (j - d_nsamples);

    // stash the tail
    memcpy(&d_tail[0], d_invfft->get_outbuf() + d_nsamples,
	   tailsize() * sizeof(gr_complex));
  }

  assert(dec_ctr == 0);

  return nitems;
}
