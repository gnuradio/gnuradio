/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
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

#include <gnuradio/filter/polyphase_filterbank.h>
#include <cstdio>

namespace gr {
  namespace filter {
    namespace kernel {

      polyphase_filterbank::polyphase_filterbank(unsigned int nfilts,
						 const std::vector<float> &taps,
                                                 bool fft_forward)
	: d_nfilts(nfilts)
    {
      d_fir_filters = std::vector<kernel::fir_filter_ccf*>(d_nfilts);
      d_fft_filters = std::vector<kernel::fft_filter_ccf*>(d_nfilts);

      // Create an FIR filter for each channel and zero out the taps
      std::vector<float> vtaps(1, 0.0f);
      for(unsigned int i = 0; i < d_nfilts; i++) {
	d_fir_filters[i] = new kernel::fir_filter_ccf(1, vtaps);
        d_fft_filters[i] = new kernel::fft_filter_ccf(1, vtaps);
      }

      // Now, actually set the filters' taps
      set_taps(taps);

      // Create the FFT to handle the output de-spinning of the channels
      d_fft = new fft::fft_complex(d_nfilts, fft_forward);
    }

    polyphase_filterbank::~polyphase_filterbank()
    {
      delete d_fft;
      for(unsigned int i = 0; i < d_nfilts; i++) {
        delete d_fir_filters[i];
        delete d_fft_filters[i];
      }
    }

    void
    polyphase_filterbank::set_taps(const std::vector<float> &taps)
    {
      unsigned int i,j;
      unsigned int ntaps = taps.size();
      d_taps_per_filter = (unsigned int)ceil((double)ntaps/(double)d_nfilts);

      // Create d_numchan vectors to store each channel's taps
      d_taps.resize(d_nfilts);

      // Make a vector of the taps plus fill it out with 0's to fill
      // each polyphase filter with exactly d_taps_per_filter
      std::vector<float> tmp_taps = taps;
      while((float)(tmp_taps.size()) < d_nfilts*d_taps_per_filter) {
        tmp_taps.push_back(0.0);
      }

      // Partition the filter
      for(i = 0; i < d_nfilts; i++) {
	// Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
	d_taps[i] = std::vector<float>(d_taps_per_filter, 0);
	for(j = 0; j < d_taps_per_filter; j++) {
	  d_taps[i][j] = tmp_taps[i + j*d_nfilts];
	}

	// Set the filter taps for each channel
	d_fir_filters[i]->set_taps(d_taps[i]);
	d_fft_filters[i]->set_taps(d_taps[i]);
      }
    }

    void
    polyphase_filterbank::print_taps()
    {
      unsigned int i, j;
      for(i = 0; i < d_nfilts; i++) {
	printf("filter[%d]: [", i);
	for(j = 0; j < d_taps_per_filter; j++) {
	  printf(" %.4e", d_taps[i][j]);
	}
	printf("]\n\n");
      }
    }

    std::vector< std::vector<float> >
    polyphase_filterbank::taps() const
    {
      return d_taps;
    }

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */
