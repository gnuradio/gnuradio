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
#include <gnuradio/io_signature.h>

namespace gr {
  namespace digital {

    using namespace filter::kernel;

    cma_equalizer_cc::sptr
    cma_equalizer_cc::make(int num_taps, float modulus, float mu, int sps)
    {
      return gnuradio::get_initial_sptr
	(new cma_equalizer_cc_impl(num_taps, modulus, mu, sps));
    }

    cma_equalizer_cc_impl::cma_equalizer_cc_impl(int num_taps, float modulus,
						 float mu, int sps)
      : sync_decimator("cma_equalizer_cc",
			  io_signature::make(1, 1, sizeof(gr_complex)),
			  io_signature::make(1, 1, sizeof(gr_complex)),
			  sps),
	fir_filter_ccc(sps, std::vector<gr_complex>(num_taps, gr_complex(0,0))),
	d_new_taps(num_taps, gr_complex(0,0)),
	d_updated(false), d_error(gr_complex(0,0))
    {
      set_modulus(modulus);
      set_gain(mu);
      if(num_taps > 0)
	d_new_taps[0] = 1.0;
      fir_filter_ccc::set_taps(d_new_taps);

      set_history(num_taps);
    }

    cma_equalizer_cc_impl::~cma_equalizer_cc_impl()
    {
    }

    void
    cma_equalizer_cc_impl::set_taps(const std::vector<gr_complex> &taps)
    {
      d_new_taps = taps;
      d_updated = true;
    }

    std::vector<gr_complex>
    cma_equalizer_cc_impl::taps() const
    {
      return d_taps;
    }

    gr_complex
    cma_equalizer_cc_impl::error(const gr_complex &out)
    { 
      gr_complex error = out*(norm(out) - d_modulus);
      float re = gr::clip(error.real(), 1.0);
      float im = gr::clip(error.imag(), 1.0);
      return gr_complex(re, im);
    }

    void
    cma_equalizer_cc_impl::update_tap(gr_complex &tap, const gr_complex &in)
    {
      // Hn+1 = Hn - mu*conj(Xn)*zn*(|zn|^2 - 1)
      tap -= d_mu*conj(in)*d_error;
    }

    int
    cma_equalizer_cc_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *)input_items[0];
      gr_complex *out = (gr_complex *)output_items[0];

      if(d_updated) {
	d_taps = d_new_taps;
	set_history(d_taps.size());
	d_updated = false;
	return 0;		     // history requirements may have changed.
      }

      int j = 0;
      size_t k, l = d_taps.size();
      for(int i = 0; i < noutput_items; i++) {
	out[i] = filter(&in[j]);

	// Adjust taps
	d_error = error(out[i]);
	for(k = 0; k < l; k++) {
	  // Update tap locally from error.
	  update_tap(d_taps[k], in[j+k]);

	  // Update aligned taps in filter object.
	  fir_filter_ccc::update_tap(d_taps[k], k);
	}

	j += decimation();
      }

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
