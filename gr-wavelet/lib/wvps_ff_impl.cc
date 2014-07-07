/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#include <wvps_ff_impl.h>
#include <gnuradio/io_signature.h>
#include <string.h>

namespace gr {
  namespace wavelet {

    static int
    ceil_log2(int k)
    {
      int m = 0;
      for (int n = k-1; n > 0; n >>= 1) m++;
      return m;
    }

    wvps_ff::sptr wvps_ff::make(int ilen)
    {
      return gnuradio::get_initial_sptr(new wvps_ff_impl(ilen));
    }

    wvps_ff_impl::wvps_ff_impl(int ilen)
      : sync_block("wvps_ff",
		      io_signature::make(1, 1, sizeof(float) * ilen),
		      io_signature::make(1, 1, sizeof(float) * ceil_log2(ilen))),
	d_ilen(ilen), d_olen(ceil_log2(ilen))
    {
    }

    // input vector assumed to be output from gsl wavelet computation

    int
    wvps_ff_impl::work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
      const float *in  = (const float *) input_items[0];
      float       *out = (float *) output_items[0];

      for (int count = 0; count < noutput_items; count++) {

	// any power?

	if (in[0] == 0.0) {
	  for (int i = 0; i < d_olen; i++)
	    out[i] = 0.0;

	} else {

	  // get power normalization from 0-th wavelet coefficient

	  float scl = 1.0/(in[0]*in[0]);
	  int k = 1;

	  // sum powers over sequences of bins,
	  // sequence lengths in increasing powers of 2

	  for (int e = 0; e < d_olen; e++) {
	    int m = 01<<e;
	    float sum = 0.0;

	    for (int l = 0; l < m; l++)
	      sum += (in[k+l]*in[k+l]);

	    out[e] = scl*sum;
	    k += m;
	  }
	}

	in  += d_ilen;
	out += d_olen;
      }

      return noutput_items;
    }

  } /* namespace wavelet */
} /* namespace gr */
