/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012 Free Software Foundation, Inc.
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

#include "pfb_channelizer_ccf_impl.h"
#include <gr_io_signature.h>

namespace gr {
  namespace filter {
    
    pfb_channelizer_ccf::sptr pfb_channelizer_ccf::make(unsigned int nfilts,
							const std::vector<float> &taps,
							float oversample_rate)
    {
      return gnuradio::get_initial_sptr(new pfb_channelizer_ccf_impl(nfilts, taps,
								     oversample_rate));
    }

    pfb_channelizer_ccf_impl::pfb_channelizer_ccf_impl(unsigned int nfilts,
						       const std::vector<float> &taps,
						       float oversample_rate)
      : gr_block("pfb_channelizer_ccf",
		 gr_make_io_signature(nfilts, nfilts, sizeof(gr_complex)),
		 gr_make_io_signature(1, nfilts, sizeof(gr_complex))),
	polyphase_filterbank(nfilts, taps),
	d_updated(false), d_oversample_rate(oversample_rate)
    {
      // The over sampling rate must be rationally related to the number of channels
      // in that it must be N/i for i in [1,N], which gives an outputsample rate
      // of [fs/N, fs] where fs is the input sample rate.
      // This tests the specified input sample rate to see if it conforms to this
      // requirement within a few significant figures.
      double intp = 0;
      double fltp = modf(nfilts / oversample_rate, &intp);
      if(fltp != 0.0)
	throw std::invalid_argument("pfb_channelizer: oversample rate must be N/i for i in [1, N]");

      set_relative_rate(1.0/intp);

      // Default channel map
      d_channel_map.resize(d_nfilts);
      for(unsigned int i = 0; i < d_nfilts; i++) {
	d_channel_map[i] = i;
      }

      // Although the filters change, we use this look up table
      // to set the index of the FFT input buffer, which equivalently
      // performs the FFT shift operation on every other turn.
      d_rate_ratio = (int)rintf(d_nfilts / d_oversample_rate);
      d_idxlut = new int[d_nfilts];
      for(unsigned int i = 0; i < d_nfilts; i++) {
	d_idxlut[i] = d_nfilts - ((i + d_rate_ratio) % d_nfilts) - 1;
      }

      // Calculate the number of filtering rounds to do to evenly
      // align the input vectors with the output channels
      d_output_multiple = 1;
      while((d_output_multiple * d_rate_ratio) % d_nfilts != 0)
	d_output_multiple++;
      set_output_multiple(d_output_multiple);

      set_history(d_taps_per_filter+1);
    }

    pfb_channelizer_ccf_impl::~pfb_channelizer_ccf_impl()
    {
      delete [] d_idxlut;
    }

    void
    pfb_channelizer_ccf_impl::set_taps(const std::vector<float> &taps)
    {
      gruel::scoped_lock guard(d_mutex);

      polyphase_filterbank::set_taps(taps);
      set_history(d_taps_per_filter+1);
      d_updated = true;
    }

    void
    pfb_channelizer_ccf_impl::print_taps()
    {
      polyphase_filterbank::print_taps();
    }

    std::vector<std::vector<float> >
    pfb_channelizer_ccf_impl::taps() const
    {
      return polyphase_filterbank::taps();
    }

    void
    pfb_channelizer_ccf_impl::set_channel_map(const std::vector<int> &map)
    {
      gruel::scoped_lock guard(d_mutex);
      
      if(map.size() > 0) {
	unsigned int max = (unsigned int)*std::max_element(map.begin(), map.end());
	if(max >= d_nfilts) {
	  throw std::invalid_argument("pfb_channelizer_ccf_impl::set_channel_map: map range out of bounds.\n");
	}
	d_channel_map = map;
      }
    }

    std::vector<int>
    pfb_channelizer_ccf_impl::channel_map() const
    {
      return d_channel_map;
    }

    int
    pfb_channelizer_ccf_impl::general_work(int noutput_items,
					   gr_vector_int &ninput_items,
					   gr_vector_const_void_star &input_items,
					   gr_vector_void_star &output_items)
    {
      gruel::scoped_lock guard(d_mutex);

      gr_complex *in = (gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      
      if(d_updated) {
	d_updated = false;
	return 0;		     // history requirements may have changed.
      }

      size_t noutputs = output_items.size();

      int n=1, i=-1, j=0, oo=0, last;
      int toconsume = (int)rintf(noutput_items/d_oversample_rate);
      while(n <= toconsume) {
	j = 0;
	i = (i + d_rate_ratio) % d_nfilts;
	last = i;
	while(i >= 0) {
	  in = (gr_complex*)input_items[j];
	  d_fft->get_inbuf()[d_idxlut[j]] = d_filters[i]->filter(&in[n]);
	  j++;
	  i--;
	}

	i = d_nfilts-1;
	while(i > last) {
	  in = (gr_complex*)input_items[j];
	  d_fft->get_inbuf()[d_idxlut[j]] = d_filters[i]->filter(&in[n-1]);
	  j++;
	  i--;
	}

	n += (i+d_rate_ratio) >= (int)d_nfilts;

	// despin through FFT
	d_fft->execute();

	// Send to output channels
	for(unsigned int nn = 0; nn < noutputs; nn++) {
	  out = (gr_complex*)output_items[nn];
	  out[oo] = d_fft->get_outbuf()[d_channel_map[nn]];
	}
	oo++;
      }

      consume_each(toconsume);
      return noutput_items;
    }

  } /* namespace filter */
} /* namespace gr */
