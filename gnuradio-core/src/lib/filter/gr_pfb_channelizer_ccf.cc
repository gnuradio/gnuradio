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
  return gnuradio::get_initial_sptr(new gr_pfb_channelizer_ccf (numchans, taps,
								  oversample_rate));
}


gr_pfb_channelizer_ccf::gr_pfb_channelizer_ccf (unsigned int numchans, 
						const std::vector<float> &taps,
						float oversample_rate)
  : gr_block ("pfb_channelizer_ccf",
	      gr_make_io_signature (numchans, numchans, sizeof(gr_complex)),
	      gr_make_io_signature (1, 1, numchans*sizeof(gr_complex))),
    d_updated (false), d_numchans(numchans), d_oversample_rate(oversample_rate)
{
  // The over sampling rate must be rationally related to the number of channels
  // in that it must be N/i for i in [1,N], which gives an outputsample rate 
  // of [fs/N, fs] where fs is the input sample rate.
  // This tests the specified input sample rate to see if it conforms to this
  // requirement within a few significant figures.
  double intp = 0;
  double fltp = modf(numchans / oversample_rate, &intp);
  if(fltp != 0.0)
    throw std::invalid_argument("gr_pfb_channelizer: oversample rate must be N/i for i in [1, N]"); 

  set_relative_rate(1.0/intp);

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

  // Although the filters change, we use this look up table
  // to set the index of the FFT input buffer, which equivalently
  // performs the FFT shift operation on every other turn.
  d_rate_ratio = (int)rintf(d_numchans / d_oversample_rate);
  d_idxlut = new int[d_numchans];
  for(unsigned int i = 0; i < d_numchans; i++) {
    d_idxlut[i] = d_numchans - ((i + d_rate_ratio) % d_numchans) - 1;
  }

  // Calculate the number of filtering rounds to do to evenly
  // align the input vectors with the output channels
  d_output_multiple = 1;
  while((d_output_multiple * d_rate_ratio) % d_numchans != 0)
    d_output_multiple++;
  set_output_multiple(d_output_multiple);
}

gr_pfb_channelizer_ccf::~gr_pfb_channelizer_ccf ()
{
  delete [] d_idxlut; 
  
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
  set_history (d_taps_per_filter+1);

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
gr_pfb_channelizer_ccf::general_work (int noutput_items,
				      gr_vector_int &ninput_items,
				      gr_vector_const_void_star &input_items,
				      gr_vector_void_star &output_items)
{
  gr_complex *in = (gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  if (d_updated) {
    d_updated = false;
    return 0;		     // history requirements may have changed.
  }

  int n=1, i=-1, j=0, last;
  int toconsume = (int)rintf(noutput_items/d_oversample_rate);
  while(n <= toconsume) {
    j = 0;
    i = (i + d_rate_ratio) % d_numchans;
    last = i;
    while(i >= 0) {
      in = (gr_complex*)input_items[j];
      d_fft->get_inbuf()[d_idxlut[j]] = d_filters[i]->filter(&in[n]);
      j++;
      i--;
    }

    i = d_numchans-1;
    while(i > last) {
      in = (gr_complex*)input_items[j];
      d_fft->get_inbuf()[d_idxlut[j]] = d_filters[i]->filter(&in[n-1]);
      j++;
      i--;
    }

    n += (i+d_rate_ratio) >= (int)d_numchans;

    // despin through FFT
    d_fft->execute();
    memcpy(out, d_fft->get_outbuf(), d_numchans*sizeof(gr_complex));
    out += d_numchans;
  }

  consume_each(toconsume);
  return noutput_items;
}
