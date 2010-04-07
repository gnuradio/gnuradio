/* -*- c++ -*- */
/*
 * Copyright 2009,2010 Free Software Foundation, Inc.
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

#include <gr_pfb_channelizer_ccf.h>
#include <gr_fir_ccf.h>
#include <gr_fir_util.h>
#include <gri_fft.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <cstring>

gr_pfb_channelizer_ccf_sptr gr_make_pfb_channelizer_ccf (unsigned int numchans, 
							 const std::vector<float> &taps,
							 float oversample_rate)
{
  return gr_pfb_channelizer_ccf_sptr (new gr_pfb_channelizer_ccf (numchans, taps,
								  oversample_rate));
}


gr_pfb_channelizer_ccf::gr_pfb_channelizer_ccf (unsigned int numchans, 
						const std::vector<float> &taps,
						float oversample_rate)
  : gr_sync_block ("pfb_channelizer_ccf",
		   gr_make_io_signature (numchans, numchans, sizeof(gr_complex)),
		   gr_make_io_signature (1, 1, numchans*sizeof(gr_complex))),
    d_updated (false), d_oversample_rate(oversample_rate)
{
  d_numchans = numchans;
  d_filters = std::vector<gr_fir_ccf*>(d_numchans);

  // Create an FIR filter for each channel and zero out the taps
  std::vector<float> vtaps(0, d_numchans);
  for(unsigned int i = 0; i < d_numchans; i++) {
    d_filters[i] = gr_fir_util::create_gr_fir_ccf(vtaps);
  }

  // Now, actually set the filters' taps
  set_taps(taps);

  // Create the FFT to handle the output de-spinning of the channels
  d_fft = new gri_fft_complex (d_numchans, false);
}

gr_pfb_channelizer_ccf::~gr_pfb_channelizer_ccf ()
{
  for(unsigned int i = 0; i < d_numchans; i++) {
    delete d_filters[i];
  }
}

void
gr_pfb_channelizer_ccf::set_taps (const std::vector<float> &taps)
{
  unsigned int i,j;

  unsigned int ntaps = taps.size();
  d_taps_per_filter = (unsigned int)ceil((double)ntaps/(double)d_numchans);

  // Create d_numchan vectors to store each channel's taps
  d_taps.resize(d_numchans);

  // Make a vector of the taps plus fill it out with 0's to fill
  // each polyphase filter with exactly d_taps_per_filter
  std::vector<float> tmp_taps;
  tmp_taps = taps;
  while((float)(tmp_taps.size()) < d_numchans*d_taps_per_filter) {
    tmp_taps.push_back(0.0);
  }
 
  // Partition the filter
  for(i = 0; i < d_numchans; i++) {
    // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
    d_taps[i] = std::vector<float>(d_taps_per_filter, 0);
    for(j = 0; j < d_taps_per_filter; j++) {
      d_taps[i][j] = tmp_taps[i + j*d_numchans];  // add taps to channels in reverse order
    }
    
    // Build a filter for each channel and add it's taps to it
    d_filters[i]->set_taps(d_taps[i]);
  }

  // Set the history to ensure enough input items for each filter
  set_history (d_taps_per_filter);

  d_updated = true;
}

void
gr_pfb_channelizer_ccf::print_taps()
{
  unsigned int i, j;
  for(i = 0; i < d_numchans; i++) {
    printf("filter[%d]: [", i);
    for(j = 0; j < d_taps_per_filter; j++) {
      printf(" %.4e", d_taps[i][j]);
    }
    printf("]\n\n");
  }
}


int
gr_pfb_channelizer_ccf::work (int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
{
  gr_complex *in = (gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  if (d_updated) {
    d_updated = false;
    return 0;		     // history requirements may have changed.
  }

  int M = d_oversample_rate;
  int N = d_numchans;
  int r = N / M;

  int n=0, i=0, j=0;
  
  printf("\nnoutput_items = %d\n", noutput_items);
  printf("N = %d   M  = %d   r = %d\n", N, M, r);

  //for(int n = 1; n < noutput_items; n++) {
  while(n < noutput_items) {
    j = 0;
    i = (i + r - 1) % N;
    //printf("i = %d   i >= 0   n = %d\n", i, n);
    while(i >= 0) {
      in = (gr_complex*)input_items[j];
      d_fft->get_inbuf()[i] = d_filters[i]->filter(&in[n]);
      j++;
      i--;
    }

    i = N;
    //printf("i = %d   r = %d   i >= r\n", i, r);
    while(i > r) {
      i--;
      in = (gr_complex*)input_items[j];
      d_fft->get_inbuf()[i] = d_filters[i]->filter(&in[n-1]);
      j++;
    }

    n += (i+r) >= N;

    /*
    // Move through filters from bottom to top
    for(int j = d_numchans-1; j >= 0; j--) {
      // Take in the items from the first input stream to d_numchans
      in = (gr_complex*)input_items[d_numchans - 1 - j];

      // Filter current input stream from bottom filter to top
      d_fft->get_inbuf()[j] = d_filters[j]->filter(&in[i]);
    }
    */

    // despin through FFT
    d_fft->execute();
    memcpy(&out[d_numchans*n], d_fft->get_outbuf(), d_numchans*sizeof(gr_complex));
    //memcpy(&out[d_numchans*i], d_fft->get_outbuf(), d_numchans*sizeof(gr_complex));
  }
  
  return noutput_items;
}
