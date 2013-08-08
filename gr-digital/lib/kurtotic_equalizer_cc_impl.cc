/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#include "kurtotic_equalizer_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace digital {

    kurtotic_equalizer_cc::sptr
    kurtotic_equalizer_cc::make(int num_taps, float mu)
    {
      return gnuradio::get_initial_sptr
	(new kurtotic_equalizer_cc_impl(num_taps, mu));
    }

    kurtotic_equalizer_cc_impl::kurtotic_equalizer_cc_impl(int num_taps, float mu)
      : sync_decimator("kurtotic_equalizer_cc",
			  io_signature::make(1, 1, sizeof(gr_complex)),
			  io_signature::make(1, 1, sizeof(gr_complex)),
			  1),
	filter::kernel::fir_filter_ccc(1, std::vector<gr_complex>(num_taps, gr_complex(0,0)))
    {
      set_gain(mu);
      if(num_taps > 0)
	d_taps[0] = 1.0;
      set_taps(d_taps);

      d_alpha_p = 0.01;
      d_alpha_q = 0.01;
      d_alpha_m = 0.01;

      d_p = 0.0f;
      d_m = 0.0f;
      d_q = gr_complex(0,0);
      d_u = gr_complex(0,0);

      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));
      set_history(num_taps+1);
    }

    kurtotic_equalizer_cc_impl::~kurtotic_equalizer_cc_impl()
    {
    }

    int
    kurtotic_equalizer_cc_impl::work(int noutput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
    {
      gr_complex *in = (gr_complex *)input_items[0];
      gr_complex *out = (gr_complex *)output_items[0];

      int j = 0, k, l = d_taps.size();
      for(int i = 0; i < noutput_items; i++) {
	out[i] = filter(&in[j]);

	// Adjust taps
	d_error = error(out[i]);
	for(k = 0; k < l; k++) {
	  update_tap(d_taps[l-k-1], in[j+k]);
	}

	j += decimation();
      }

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
