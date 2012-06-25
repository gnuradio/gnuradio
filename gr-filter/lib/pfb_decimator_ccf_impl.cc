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

#include "pfb_decimator_ccf_impl.h"
#include <gr_io_signature.h>

namespace gr {
  namespace filter {
    
    pfb_decimator_ccf::sptr
    pfb_decimator_ccf::make(unsigned int decim,
			    const std::vector<float> &taps,
			    unsigned int channel)
    {
      return gnuradio::get_initial_sptr
	(new pfb_decimator_ccf_impl(decim, taps, channel));
    }


    pfb_decimator_ccf_impl::pfb_decimator_ccf_impl(unsigned int decim,
						   const std::vector<float> &taps,
						   unsigned int channel)
      : gr_sync_block("pfb_decimator_ccf",
		      gr_make_io_signature(decim, decim, sizeof(gr_complex)),
		      gr_make_io_signature(1, 1, sizeof(gr_complex))),
	polyphase_filterbank(decim, taps),
	d_updated(false), d_chan(channel)
    {
      d_rate = decim;
      d_rotator = new gr_complex[d_rate];

      set_relative_rate(1.0/(float)decim);
      set_history(d_taps_per_filter+1);
    }

    pfb_decimator_ccf_impl::~pfb_decimator_ccf_impl()
    {
    }

    void
    pfb_decimator_ccf_impl::set_taps(const std::vector<float> &taps)
    {
      gruel::scoped_lock guard(d_mutex);

      polyphase_filterbank::set_taps(taps);
      set_history(d_taps_per_filter+1);
      d_updated = true;
    }

    void
    pfb_decimator_ccf_impl::print_taps()
    {
      polyphase_filterbank::print_taps();
    }

    std::vector<std::vector<float> >
    pfb_decimator_ccf_impl::taps() const
    {
      return polyphase_filterbank::taps();
    }

#define ROTATEFFT

    int
    pfb_decimator_ccf_impl::work(int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
    {
      gruel::scoped_lock guard(d_mutex);

      gr_complex *in;
      gr_complex *out = (gr_complex *)output_items[0];

      if(d_updated) {
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

  } /* namespace filter */
} /* namespace gr */
