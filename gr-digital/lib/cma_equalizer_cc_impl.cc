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

#include "cma_equalizer_cc_impl.h"
#include <gr_io_signature.h>

namespace gr {
  namespace digital {

    cma_equalizer_cc::sptr
    cma_equalizer_cc::make(int num_taps, float modulus, float mu, int sps)
    {
      return gnuradio::get_initial_sptr
	(new cma_equalizer_cc_impl(num_taps, modulus, mu, sps));
    }

    cma_equalizer_cc_impl::cma_equalizer_cc_impl(int num_taps, float modulus,
						 float mu, int sps)
      : gr_sync_decimator("cma_equalizer_cc",
			  gr_make_io_signature(1, 1, sizeof(gr_complex)),
			  gr_make_io_signature(1, 1, sizeof(gr_complex)),
			  sps),
	filter::kernel::fir_filter_ccc(sps, std::vector<gr_complex>(num_taps, gr_complex(0,0))),
	d_updated(false), d_error(gr_complex(0,0))
    {
      set_modulus(modulus);
      set_gain(mu);
      if(num_taps > 0)
	d_new_taps[0] = 1.0;
      filter::kernel::fir_filter_ccc::set_taps(d_new_taps);

      set_history(ntaps());
    }

    cma_equalizer_cc_impl::~cma_equalizer_cc_impl()
    {
    }

    int
    cma_equalizer_cc_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      gr_complex *in = (gr_complex *)input_items[0];
      gr_complex *out = (gr_complex *)output_items[0];

      if (d_updated) {
	filter::kernel::fir_filter_ccc::set_taps(d_new_taps);
	set_history(ntaps());
	d_updated = false;
	return 0;		     // history requirements may have changed.
      }

      int j = 0, k, l = d_taps.size();
      for(int i = 0; i < noutput_items; i++) {
	out[i] = filter(&in[j]);

	// Adjust taps
	d_error = error(out[i]);
	for (k = 0; k < l; k++) {
	  update_tap(d_taps[l-k-1], in[j+k]);
	}

	j += decimation();
      }

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
