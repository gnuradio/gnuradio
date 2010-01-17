/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#include <gr_pfb_arb_resampler_ccf.h>
#include <gr_fir_ccf.h>
#include <gr_fir_util.h>
#include <gr_io_signature.h>
#include <cstdio>

gr_pfb_arb_resampler_ccf_sptr gr_make_pfb_arb_resampler_ccf (float rate, 
							     const std::vector<float> &taps,
							     unsigned int filter_size)
{
  return gr_pfb_arb_resampler_ccf_sptr (new gr_pfb_arb_resampler_ccf (rate, taps,
								      filter_size));
}


gr_pfb_arb_resampler_ccf::gr_pfb_arb_resampler_ccf (float rate, 
						    const std::vector<float> &taps,
						    unsigned int filter_size)
  : gr_block ("pfb_arb_resampler_ccf",
	      gr_make_io_signature (1, 1, sizeof(gr_complex)),
	      gr_make_io_signature (1, 1, sizeof(gr_complex))),
    d_updated (false)
{
  /* The number of filters is specified by the user as the filter size;
     this is also the interpolation rate of the filter. We use it and the
     rate provided to determine the decimation rate. This acts as a
     rational resampler. The flt_rate is calculated as the residual
     between the integer decimation rate and the real decimation rate and
     will be used to determine to interpolation point of the resampling
     process.
  */
  d_int_rate = filter_size;
  d_dec_rate = (unsigned int)floor(d_int_rate/rate);
  d_flt_rate = (d_int_rate/rate) - d_dec_rate;

  // Store the last filter between calls to work
  d_last_filter = 0;

  d_start_index = 0;
  
  d_filters = std::vector<gr_fir_ccf*>(d_int_rate);
  d_diff_filters = std::vector<gr_fir_ccf*>(d_int_rate);

  // Create an FIR filter for each channel and zero out the taps
  std::vector<float> vtaps(0, d_int_rate);
  for(int i = 0; i < d_int_rate; i++) {
    d_filters[i] = gr_fir_util::create_gr_fir_ccf(vtaps);
    d_diff_filters[i] = gr_fir_util::create_gr_fir_ccf(vtaps);
  }

  // Now, actually set the filters' taps
  std::vector<float> dtaps;
  create_diff_taps(taps, dtaps);
  set_taps(taps, d_taps, d_filters);
  set_taps(dtaps, d_dtaps, d_diff_filters);
}

gr_pfb_arb_resampler_ccf::~gr_pfb_arb_resampler_ccf ()
{
  for(unsigned int i = 0; i < d_int_rate; i++) {
    delete d_filters[i];
  }
}

void
gr_pfb_arb_resampler_ccf::set_taps (const std::vector<float> &newtaps,
				    std::vector< std::vector<float> > &ourtaps,
				    std::vector<gr_fir_ccf*> &ourfilter)
{
  int i,j;

  unsigned int ntaps = newtaps.size();
  d_taps_per_filter = (unsigned int)ceil((double)ntaps/(double)d_int_rate);

  // Create d_numchan vectors to store each channel's taps
  ourtaps.resize(d_int_rate);
  
  // Make a vector of the taps plus fill it out with 0's to fill
  // each polyphase filter with exactly d_taps_per_filter
  std::vector<float> tmp_taps;
  tmp_taps = newtaps;
  while((float)(tmp_taps.size()) < d_int_rate*d_taps_per_filter) {
    tmp_taps.push_back(0.0);
  }
  
  // Partition the filter
  for(i = 0; i < d_int_rate; i++) {
    // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
    ourtaps[d_int_rate-1-i] = std::vector<float>(d_taps_per_filter, 0);
    for(j = 0; j < d_taps_per_filter; j++) {
      ourtaps[d_int_rate - 1 - i][j] = tmp_taps[i + j*d_int_rate];
    }
    
    // Build a filter for each channel and add it's taps to it
    ourfilter[i]->set_taps(ourtaps[d_int_rate-1-i]);
  }

  // Set the history to ensure enough input items for each filter
  set_history (d_taps_per_filter + 1);

  d_updated = true;
}

void
gr_pfb_arb_resampler_ccf::create_diff_taps(const std::vector<float> &newtaps,
					   std::vector<float> &difftaps)
{
  float maxtap = 1e-20;
  difftaps.clear();
  difftaps.push_back(0); //newtaps[0]);
  for(unsigned int i = 1; i < newtaps.size()-1; i++) {
    float tap = newtaps[i+1] - newtaps[i-1];
    difftaps.push_back(tap);
    if(tap > maxtap) {
      maxtap = tap;
    }
  }
  difftaps.push_back(0);//-newtaps[newtaps.size()-1]);

  // Scale the differential taps; helps scale error term to better update state
  // FIXME: should this be scaled this way or use the same gain as the taps?
  for(unsigned int i = 0; i < difftaps.size(); i++) {
    difftaps[i] /= maxtap;
  }
}

void
gr_pfb_arb_resampler_ccf::print_taps()
{
  unsigned int i, j;
  for(i = 0; i < d_int_rate; i++) {
    printf("filter[%d]: [", i);
    for(j = 0; j < d_taps_per_filter; j++) {
      printf(" %.4e", d_taps[i][j]);
    }
    printf("]\n");
  }
}

int
gr_pfb_arb_resampler_ccf::general_work (int noutput_items,
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

  int i = 0, j, count = d_start_index;
  gr_complex o0, o1;

  // Restore the last filter position
  j = d_last_filter;

  // produce output as long as we can and there are enough input samples
  while((i < noutput_items) && (count < ninput_items[0]-1)) {

    // start j by wrapping around mod the number of channels
    while((j < d_int_rate) && (i < noutput_items)) {
      // Take the current filter output
      o0 = d_filters[j]->filter(&in[count]);
      o1 = d_diff_filters[j]->filter(&in[count]);

      out[i] = o0 + o1*d_flt_rate;     // linearly interpolate between samples
      i++;
      
      j += d_dec_rate;
    }
    if(i < noutput_items) {              // keep state for next entry
      float ss = (int)(j / d_int_rate);  // number of items to skip ahead by
      count += ss;                       // we have fully consumed another input
      j = j % d_int_rate;                // roll filter around
    }
  }

  // Store the current filter position and start of next sample
  d_last_filter = j;
  d_start_index = std::max(0, count - ninput_items[0]);

  // consume all we've processed but no more than we can
  consume_each(std::min(count, ninput_items[0]));
  return i;
}
