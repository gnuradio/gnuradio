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

#include <gr_pfb_decimator_ccf.h>
#include <gr_fir_ccf.h>
#include <gr_fir_util.h>
#include <gri_fft.h>
#include <gr_io_signature.h>
#include <gr_expj.h>
#include <cstdio>

gr_pfb_decimator_ccf_sptr gr_make_pfb_decimator_ccf (unsigned int decim, 
						     const std::vector<float> &taps,
						     unsigned int channel)
{
  return gnuradio::get_initial_sptr(new gr_pfb_decimator_ccf (decim, taps, channel));
}


gr_pfb_decimator_ccf::gr_pfb_decimator_ccf (unsigned int decim, 
					    const std::vector<float> &taps,
					    unsigned int channel)
  : gr_sync_block ("pfb_decimator_ccf",
		   gr_make_io_signature (decim, decim, sizeof(gr_complex)),
		   gr_make_io_signature (1, 1, sizeof(gr_complex))),
    d_updated (false)
{
  d_rate = decim;
  d_filters = std::vector<gr_fir_ccf*>(d_rate);
  d_chan = channel;
  d_rotator = new gr_complex[d_rate];

  // Create an FIR filter for each channel and zero out the taps
  std::vector<float> vtaps(0, d_rate);
  for(unsigned int i = 0; i < d_rate; i++) {
    d_filters[i] = gr_fir_util::create_gr_fir_ccf(vtaps);
    d_rotator[i] = gr_expj(i*2*M_PI*d_chan/d_rate);
  }

  // Now, actually set the filters' taps
  set_taps(taps);

  // Create the FFT to handle the output de-spinning of the channels
  d_fft = new gri_fft_complex (d_rate, false);
}

gr_pfb_decimator_ccf::~gr_pfb_decimator_ccf ()
{
  for(unsigned int i = 0; i < d_rate; i++) {
    delete d_filters[i];
  }
}

void
gr_pfb_decimator_ccf::set_taps (const std::vector<float> &taps)
{
  unsigned int i,j;

  unsigned int ntaps = taps.size();
  d_taps_per_filter = (unsigned int)ceil((double)ntaps/(double)d_rate);

  // Create d_numchan vectors to store each channel's taps
  d_taps.resize(d_rate);

  // Make a vector of the taps plus fill it out with 0's to fill
  // each polyphase filter with exactly d_taps_per_filter
  std::vector<float> tmp_taps;
  tmp_taps = taps;
  while((float)(tmp_taps.size()) < d_rate*d_taps_per_filter) {
    tmp_taps.push_back(0.0);
  }
  
  // Partition the filter
  for(i = 0; i < d_rate; i++) {
    // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
    d_taps[i] = std::vector<float>(d_taps_per_filter, 0);
    for(j = 0; j < d_taps_per_filter; j++) {
      d_taps[i][j] = tmp_taps[i + j*d_rate];  // add taps to channels in reverse order
    }
    
    // Build a filter for each channel and add it's taps to it
    d_filters[i]->set_taps(d_taps[i]);
  }

  // Set the history to ensure enough input items for each filter
  set_history (d_taps_per_filter);

  d_updated = true;
}

void
gr_pfb_decimator_ccf::print_taps()
{
  unsigned int i, j;
  for(i = 0; i < d_rate; i++) {
    printf("filter[%d]: [", i);
    for(j = 0; j < d_taps_per_filter; j++) {
      printf(" %.4e", d_taps[i][j]);
    }
    printf("]\n\n");
  }
}

#define ROTATEFFT

int
gr_pfb_decimator_ccf::work (int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
{
  gr_complex *in;
  gr_complex *out = (gr_complex *) output_items[0];

  if (d_updated) {
    d_updated = false;
    return 0;		     // history requirements may have changed.
  }

  int i;
  for(i = 0; i < noutput_items; i++) {
    // Move through filters from bottom to top
    out[i] = 0;
    for(int j = d_rate-1; j >= 0; j--) {
      // Take in the items from the first input stream to d_rate
      in = (gr_complex*)input_items[d_rate - 1 - j];

      // Filter current input stream from bottom filter to top
      // The rotate them by expj(j*k*2pi/M) where M is the number of filters
      // (the decimation rate) and k is the channel number to extract
      
      // This is the real math that goes on; we abuse the FFT to do this quickly
      // for decimation rates > N where N is a small number (~5):
      //       out[i] += d_filters[j]->filter(&in[i])*gr_expj(j*d_chan*2*M_PI/d_rate);
#ifdef ROTATEFFT
      d_fft->get_inbuf()[j] = d_filters[j]->filter(&in[i]);
#else
      out[i] += d_filters[j]->filter(&in[i])*d_rotator[i];
#endif
    }

#ifdef ROTATEFFT
    // Perform the FFT to do the complex multiply despinning for all channels
    d_fft->execute();

    // Select only the desired channel out
    out[i] = d_fft->get_outbuf()[d_chan];
#endif
    
  }
  
  return noutput_items;
}
