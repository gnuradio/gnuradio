/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#include <gr_adaptive_fir_ccc.h>
#include <gr_io_signature.h>

gr_adaptive_fir_ccc::gr_adaptive_fir_ccc(const char *name, int decimation,
					 const std::vector<gr_complex> &taps)
  : gr_sync_decimator (name,
		       gr_make_io_signature (1, 1, sizeof(gr_complex)),
		       gr_make_io_signature (1, 1, sizeof(gr_complex)),
		       decimation),
    d_updated(false), d_taps(taps)
{
  set_history(d_taps.size());
}

void
gr_adaptive_fir_ccc::set_taps(const std::vector<gr_complex> &taps)
{
  d_new_taps = taps;
  d_updated = true;
}

gr_complex
gr_adaptive_fir_ccc::filter(gr_complex *x)
{
  // Generic dot product of d_taps[] and in[]
  gr_complex acc(0.0, 0.0);
  int l = d_taps.size();
  for (int k = 0; k < l; k++)
    acc += d_taps[l-k-1] * x[k];
  return acc;
}

int
gr_adaptive_fir_ccc::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  gr_complex *in = (gr_complex *)input_items[0];
  gr_complex *out = (gr_complex *)output_items[0];

  if (d_updated) {
    d_taps = d_new_taps;
    set_history(d_taps.size());
    d_updated = false;
    return 0;		     // history requirements may have changed.
  }
  
  int j = 0, k, l = d_taps.size();
  for (int i = 0; i < noutput_items; i++) {
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
