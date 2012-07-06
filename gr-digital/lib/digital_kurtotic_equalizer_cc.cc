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

#include <digital_kurtotic_equalizer_cc.h>
#include <gr_io_signature.h>

digital_kurtotic_equalizer_cc_sptr
digital_make_kurtotic_equalizer_cc(int num_taps, float mu)
{
  return gnuradio::get_initial_sptr
    (new digital_kurtotic_equalizer_cc(num_taps, mu));
}

digital_kurtotic_equalizer_cc::digital_kurtotic_equalizer_cc(int num_taps, float mu)
  : gr_sync_decimator("kurtotic_equalizer_cc",
		      gr_make_io_signature(1, 1, sizeof(gr_complex)),
		      gr_make_io_signature(1, 1, sizeof(gr_complex)),
		      1),
    gr::filter::kernel::adaptive_fir_ccc(1, std::vector<gr_complex>(num_taps, gr_complex(0,0)))
{
  set_gain(mu);
  if (num_taps > 0)
    d_taps[0] = 1.0;

  d_alpha_p = 0.01;
  d_alpha_q = 0.01;
  d_alpha_m = 0.01;

  d_p = 0.0f;
  d_m = 0.0f;
  d_q = gr_complex(0,0);
  d_u = gr_complex(0,0);
}

int
digital_kurtotic_equalizer_cc::work(int noutput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
{
  gr_complex *in = (gr_complex *)input_items[0];
  gr_complex *out = (gr_complex *)output_items[0];

  if(d_updated) {
    gr::filter::kernel::fir_filter_ccc::set_taps(d_new_taps);
    set_history(d_ntaps);
    d_updated = false;
    return 0;		     // history requirements may have changed.
  }

  // Call base class filtering function that uses
  // overloaded error and update_tap functions.
  if(decimation() == 1) {
    filterN(out, in, noutput_items);
  }
  else {
    filterNdec(out, in, noutput_items,
	       decimation());
  }

  return noutput_items;
}
