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

#include <digital_lms_dd_equalizer_cc.h>
#include <gr_io_signature.h>
#include <gr_misc.h>
#include <volk/volk.h>

digital_lms_dd_equalizer_cc_sptr
digital_make_lms_dd_equalizer_cc(int num_taps, float mu, int sps,
				 digital_constellation_sptr cnst)
{
  return gnuradio::get_initial_sptr
    (new digital_lms_dd_equalizer_cc(num_taps, mu, sps, cnst));
}

digital_lms_dd_equalizer_cc::digital_lms_dd_equalizer_cc(int num_taps, float mu,
							 int sps,
							 digital_constellation_sptr cnst)
  : gr_sync_decimator("lms_dd_equalizer_cc",
		      gr_make_io_signature(1, 1, sizeof(gr_complex)),
		      gr_make_io_signature(1, 1, sizeof(gr_complex)),
		      sps),
    gr::filter::kernel::adaptive_fir_ccc(sps, std::vector<gr_complex>(num_taps, gr_complex(0,0))),
    d_cnst(cnst)
{
  set_gain(mu);
  if(num_taps > 0)
    d_taps[0] = 1.0;
  set_taps(d_taps);

  const int alignment_multiple =
    volk_get_alignment() / sizeof(gr_complex);
  set_alignment(std::max(1,alignment_multiple));
  set_history(num_taps+1);
}

int
digital_lms_dd_equalizer_cc::work(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *)input_items[0];
  gr_complex *out = (gr_complex *)output_items[0];

  int j = 0;
  size_t l = d_taps.size();
  for(int i = 0; i < noutput_items; i++) {
    out[i] = filter(&in[j]);

    d_error = error(out[i]);
    for(size_t k=0; k < l; k++) {
      update_tap(d_taps[k], in[i+k]);
    }

    j += decimation();
  }

  return noutput_items;
}
