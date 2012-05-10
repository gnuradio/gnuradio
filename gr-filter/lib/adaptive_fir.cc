/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <filter/adaptive_fir.h>
#include <fft/fft.h>
#include <volk/volk.h>

namespace gr {
  namespace filter {
    namespace kernel {

      adaptive_fir_ccc::adaptive_fir_ccc(int decimation,
					 const std::vector<gr_complex> &taps)
      {
	d_taps = NULL;
	d_decim = decimation;
	set_taps(taps);
      }

      void
      adaptive_fir_ccc::set_taps(const std::vector<gr_complex> &taps)
      {
	// Free the taps if already allocated
	if(d_taps != NULL) {
	  fft::free(d_taps);
	  d_taps = NULL;
	}
	
	d_ntaps = (int)taps.size();
	d_taps = fft::malloc_complex(d_ntaps);
	for(unsigned int i = 0; i < d_ntaps; i++) {
	  d_taps[d_ntaps-i-1] = taps[i];
	}
      }

      std::vector<gr_complex>
      adaptive_fir_ccc::taps() const
      {
	std::vector<gr_complex> t;
	for(unsigned int i = 0; i < d_ntaps; i++)
	  t.push_back(d_taps[d_ntaps-i-1]);
	return t;
      }

      int
      adaptive_fir_ccc::decimation() const
      {
	return d_decim;
      }

      unsigned int
      adaptive_fir_ccc::ntaps() const
      {
	return d_ntaps;
      }

      gr_complex
      adaptive_fir_ccc::filter(gr_complex *input)
      {
	gr_complex output;
	volk_32fc_x2_dot_prod_32fc_u(&output, input, d_taps, d_ntaps);
	return output;
      }

      void
      adaptive_fir_ccc::filterN(gr_complex *out, gr_complex *in,
				int nitems)
      {
	int j = 0;
	unsigned int k;
	for(int i = 0; i < nitems; i++) {
	  out[i] = filter(&in[j]);

	  // Adjust taps
	  d_error = error(out[i]);
	  for(k = 0; k < d_ntaps; k++) {
	    update_tap(d_taps[d_ntaps-k-1], in[j+k]);
	  }

	  j += decimation();
	}
      }

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */
