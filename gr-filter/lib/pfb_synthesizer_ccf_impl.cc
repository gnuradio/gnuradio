/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
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

#include "pfb_synthesizer_ccf_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>

namespace gr {
  namespace filter {

    pfb_synthesizer_ccf::sptr
    pfb_synthesizer_ccf::make(unsigned int numchans,
			      const std::vector<float> &taps,
			      bool twox)
    {
      return gnuradio::get_initial_sptr
	(new pfb_synthesizer_ccf_impl(numchans, taps, twox));
    }


    pfb_synthesizer_ccf_impl::pfb_synthesizer_ccf_impl(unsigned int numchans,
						       const std::vector<float> &taps,
						       bool twox)
      : sync_interpolator("pfb_synthesizer_ccf",
			     io_signature::make(1, numchans, sizeof(gr_complex)),
			     io_signature::make(1, 1, sizeof(gr_complex)),
			     (twox ? numchans/2 : numchans)),
	d_updated(false), d_numchans(numchans), d_state(0)
    {
      // set up 2x multiplier; if twox==True, set to 2, otherwise to 1
      d_twox = (twox ? 2 : 1);
      if(d_numchans % d_twox != 0) {
	throw std::invalid_argument("pfb_synthesizer_ccf: number of channels must be even for 2x oversampling.\n");
      }

      d_filters = std::vector<kernel::fir_filter_with_buffer_ccf*>(d_numchans);
      d_channel_map.resize(d_numchans);

      // Create a FIR filter for each channel and zero out the taps
      std::vector<float> vtaps(0, d_numchans);
      for(unsigned int i = 0; i < d_numchans; i++) {
	d_filters[i] = new kernel::fir_filter_with_buffer_ccf(vtaps);
	d_channel_map[i] = i;
      }

      // Now, actually set the filters' taps
      set_taps(taps);

      // Create the IFFT to handle the input channel rotations
      d_fft = new fft::fft_complex(d_numchans, false);
      memset(d_fft->get_inbuf(), 0, d_numchans*sizeof(gr_complex));

      set_output_multiple(d_numchans);
    }

    pfb_synthesizer_ccf_impl::~pfb_synthesizer_ccf_impl()
    {
      delete d_fft;
      for(unsigned int i = 0; i < d_numchans; i++) {
	delete d_filters[i];
      }
    }

    void
    pfb_synthesizer_ccf_impl::set_taps(const std::vector<float> &taps)
    {
      gr::thread::scoped_lock guard(d_mutex);

      if(d_twox == 1)
	set_taps1(taps);
      else
	set_taps2(taps);

      // Set the history to ensure enough input items for each filter
      set_history(d_taps_per_filter+1);

      d_updated = true;
    }

    void
    pfb_synthesizer_ccf_impl::set_taps1(const std::vector<float> &taps)
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
    }

    void
    pfb_synthesizer_ccf_impl::set_taps2 (const std::vector<float> &taps)
    {
      unsigned int i,j;
      int state = 0;

      unsigned int ntaps = 2*taps.size();
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
      unsigned int halfchans = d_numchans/2;
      for(i = 0; i < halfchans; i++) {
	// Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
	d_taps[i] = std::vector<float>(d_taps_per_filter, 0);
	d_taps[halfchans+i] = std::vector<float>(d_taps_per_filter, 0);
	state = 0;
	for(j = 0; j < d_taps_per_filter; j++) {
	  // add taps to channels in reverse order
	  // Zero out every other tap
	  if(state == 0) {
	    d_taps[i][j] = tmp_taps[i + j*halfchans];
	    d_taps[halfchans + i][j] = 0;
	    state = 1;
	  }
	  else {
	    d_taps[i][j] = 0;
	    d_taps[halfchans + i][j] = tmp_taps[i + j*halfchans];
	    state = 0;
	  }
	}

	// Build a filter for each channel and add it's taps to it
	d_filters[i]->set_taps(d_taps[i]);
	d_filters[halfchans + i]->set_taps(d_taps[halfchans + i]);
      }
    }

    void
    pfb_synthesizer_ccf_impl::print_taps()
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

    std::vector< std::vector<float> >
    pfb_synthesizer_ccf_impl::taps() const
    {
      return d_taps;
    }

    void
    pfb_synthesizer_ccf_impl::set_channel_map(const std::vector<int> &map)
    {
      gr::thread::scoped_lock guard(d_mutex);

      if(map.size() > 0) {
	unsigned int max = (unsigned int)*std::max_element(map.begin(), map.end());
	if(max >= d_numchans) {
	  throw std::invalid_argument("gr_pfb_synthesizer_ccf::set_channel_map: map range out of bounds.\n");
	}
	d_channel_map = map;

	// Zero out fft buffer so that unused channels are always 0
	memset(d_fft->get_inbuf(), 0, d_numchans*sizeof(gr_complex));
      }
    }

    std::vector<int>
    pfb_synthesizer_ccf_impl::channel_map() const
    {
      return d_channel_map;
    }

    int
    pfb_synthesizer_ccf_impl::work(int noutput_items,
				   gr_vector_const_void_star &input_items,
				   gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_mutex);

      gr_complex *in = (gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      if(d_updated) {
	d_updated = false;
	return 0;		     // history requirements may have changed.
      }

      unsigned int n, i;
      size_t ninputs = input_items.size();

      // Algoritm for critically sampled channels
      if(d_twox == 1) {
	for(n = 0; n < noutput_items/d_numchans; n++) {
	  for(i = 0; i < ninputs; i++) {
	    in = (gr_complex*)input_items[i];
	    d_fft->get_inbuf()[d_channel_map[i]] = in[n];
	  }

	  // spin through IFFT
	  d_fft->execute();

	  for(i = 0; i < d_numchans; i++) {
	    out[i]  = d_filters[i]->filter(d_fft->get_outbuf()[i]);
	  }
	  out += d_numchans;
	}
      }

      // Algorithm for oversampling by 2x
      else {
        unsigned int halfchans = d_numchans/2;
	for(n = 0; n < noutput_items/halfchans; n++) {
	  for(i = 0; i < ninputs; i++) {
	    in = (gr_complex*)input_items[i];
	    d_fft->get_inbuf()[d_channel_map[i]] = in[n];
	  }

	  // spin through IFFT
	  d_fft->execute();

	  // Output is sum of two filters, but the input buffer to the filters must be circularly
	  // shifted by numchans every time through, done by using d_state to determine which IFFT
	  // buffer position to pull from.
	  for(i = 0; i < halfchans; i++) {
	    out[i]  = d_filters[i]->filter(d_fft->get_outbuf()[d_state*halfchans+i]);
	    out[i] += d_filters[halfchans+i]->filter(d_fft->get_outbuf()[(d_state^1)*halfchans+i]);
	  }
	  d_state ^= 1;

	  out += halfchans;
	}
      }

      return noutput_items;
    }

  } /* namespace filter */
} /* namespace gr */
