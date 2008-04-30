/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#include <gr_io_signature.h>
#include <gri_fft.h>
#include <math.h>
#include <assert.h>
#include <stdexcept>
#include <gr_firdes.h>

#include <iostream>
#include <string.h>

gr_fft_filter_fff_sptr gr_make_fft_filter_fff (int decimation, const std::vector<float> &taps)
{
  return gr_fft_filter_fff_sptr (new gr_fft_filter_fff (decimation, taps));
}


gr_fft_filter_fff::gr_fft_filter_fff (int decimation, const std::vector<float> &taps)
  : gr_sync_decimator ("fft_filter_fff",
		       gr_make_io_signature (1, 1, sizeof (float)),
		       gr_make_io_signature (1, 1, sizeof (float)),
		       decimation),
    d_fftsize(-1), d_fwdfft(0), d_invfft(0), d_updated(false)
{
  set_history(1);
  actual_set_taps(taps);
}

gr_fft_filter_fff::~gr_fft_filter_fff ()
{
  delete d_fwdfft;
  delete d_invfft;
}

static void 
print_vector_complex(const std::string label, const std::vector<gr_complex> &x)
{
  std::cout << label;
  for (unsigned i = 0; i < x.size(); i++)
    std::cout << x[i] << " ";
  std::cout << "\n";
}

static void 
print_vector_float(const std::string label, const std::vector<float> &x)
{
  std::cout << label;
  for (unsigned i = 0; i < x.size(); i++)
    std::cout << x[i] << " ";
  std::cout << "\n";
}

void
gr_fft_filter_fff::set_taps (const std::vector<float> &taps)
{
  d_new_taps = taps;
  d_updated = true;
}

/*
 * determines d_ntaps, d_nsamples, d_fftsize, d_xformed_taps
 */
void
gr_fft_filter_fff::actual_set_taps (const std::vector<float> &taps)
{
  int i = 0;
  compute_sizes(taps.size());

  d_tail.resize(tailsize());
  for (i = 0; i < tailsize(); i++)
    d_tail[i] = 0;

  float *in = d_fwdfft->get_inbuf();
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
  for (i = 0; i < d_fftsize/2+1; i++)
    d_xformed_taps[i] = out[i];

  //print_vector_complex("transformed taps:", d_xformed_taps);
}

// determine and set d_ntaps, d_nsamples, d_fftsize

void
gr_fft_filter_fff::compute_sizes(int ntaps)
{
  int old_fftsize = d_fftsize;
  d_ntaps = ntaps;
  d_fftsize = (int) (2 * pow(2.0, ceil(log(ntaps) / log(2))));
  d_nsamples = d_fftsize - d_ntaps + 1;

  if (0)
    fprintf(stderr, "gr_fft_filter: ntaps = %d, fftsize = %d, nsamples = %d\n",
	    d_ntaps, d_fftsize, d_nsamples);

  assert(d_fftsize == d_ntaps + d_nsamples -1 );

  if (d_fftsize != old_fftsize){	// compute new plans
    delete d_fwdfft;
    delete d_invfft;
    d_fwdfft = new gri_fft_real_fwd(d_fftsize);
    d_invfft = new gri_fft_real_rev(d_fftsize);
    d_xformed_taps.resize(d_fftsize/2+1);
  }

  set_output_multiple(d_nsamples);
}

int
gr_fft_filter_fff::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];

  if (d_updated){
    actual_set_taps(d_new_taps);
    d_updated = false;
    return 0;				// output multiple may have changed
  }

  assert(noutput_items % d_nsamples == 0);

  int dec_ctr = 0;
  int j = 0;
  int ninput_items = noutput_items * decimation();

  for (int i = 0; i < ninput_items; i += d_nsamples){
    
    memcpy(d_fwdfft->get_inbuf(), &in[i], d_nsamples * sizeof(float));

    for (j = d_nsamples; j < d_fftsize; j++)
      d_fwdfft->get_inbuf()[j] = 0;

    d_fwdfft->execute();	// compute fwd xform

    gr_complex *a = d_fwdfft->get_outbuf();
    gr_complex *b = &d_xformed_taps[0];
    gr_complex *c = d_invfft->get_inbuf();

    for (j = 0; j < d_fftsize/2+1; j++)	// filter in the freq domain
      c[j] = a[j] * b[j];
    
    d_invfft->execute();	// compute inv xform

    // add in the overlapping tail

    for (j = 0; j < tailsize(); j++)
      d_invfft->get_outbuf()[j] += d_tail[j];

    // copy nsamples to output

    //memcpy(out, d_invfft->get_outbuf(), d_nsamples * sizeof(float));
    //out += d_nsamples;

    j = dec_ctr;
    while (j < d_nsamples) {
      *out++ = d_invfft->get_outbuf()[j];
      j += decimation();
    }
    dec_ctr = (j - d_nsamples);

    // stash the tail
    memcpy(&d_tail[0], d_invfft->get_outbuf() + d_nsamples,
	   tailsize() * sizeof(float));
  }

  assert((out - (float *) output_items[0]) == noutput_items);
  assert(dec_ctr == 0);

  return noutput_items;
}
