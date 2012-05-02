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

#include "fir_filter_fff_impl.h"
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <fft/fft.h>
#include <gr_math.h>

namespace gr {
  namespace filter {
    
    fir_filter_fff::sptr
    fir_filter_fff::make(int decimation, const std::vector<float> &taps)
    {
      return gnuradio::get_initial_sptr(new fir_filter_fff_impl
					(decimation, taps));
    }


    fir_filter_fff_impl::fir_filter_fff_impl(int decimation,
					     const std::vector<float> &taps)
      : gr_sync_decimator("fir_filter_fff",
			  gr_make_io_signature (1, 1, sizeof(float)),
			  gr_make_io_signature (1, 1, sizeof(float)),
			  decimation)
    {
      d_taps = NULL;
      set_taps(taps);
      d_updated = false;
      set_history(d_ntaps+1);
    }

    fir_filter_fff_impl::~fir_filter_fff_impl()
    {
      if(d_taps != NULL) {
	fft::free(d_taps);
	d_taps = NULL;
      }
    }

    void
    fir_filter_fff_impl::set_taps(const std::vector<float> &taps)
    {
      // Free the taps if already allocated
      if(d_taps != NULL) {
	fft::free(d_taps);
	d_taps = NULL;
      }

      d_ntaps = (int)taps.size();
      d_taps = fft::malloc_float(d_ntaps);
      for(int i = 0; i < d_ntaps; i++) {
	d_taps[i] = taps[i];
      }
      d_updated = true;
    }
    
    std::vector<float>
    fir_filter_fff_impl::taps() const
    {
      std::vector<float> t;
      for(int i = 0; i < d_ntaps; i++)
	t.push_back(d_taps[i]);
      return t;
    }

    int
    fir_filter_fff_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      const float *in  = (const float*)input_items[0];
      float *out = (float*)output_items[0];
      
      if (d_updated) {
	set_history((unsigned int)d_ntaps+1);
	d_updated = false;
	return 0;	     // history requirements may have changed.
      }
      
      if (decimation() == 1) {
	for(int i = 0; i < noutput_items; i++) {
	  volk_32f_x2_dot_prod_32f_u(&out[i], &in[i], d_taps, d_ntaps);
	}
      }
      else {
	int j = 0;
	for(int i = 0; i < noutput_items; i++) {
	  volk_32f_x2_dot_prod_32f_u(&out[i], &in[j], d_taps, d_ntaps);
	  j += decimation();
	}
      }
      
      return noutput_items;
    }

  } /* namespace filter */
} /* namespace gr */

