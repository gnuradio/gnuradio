/* -*- c++ -*- */
/*
 * Copyright 2007,2010 Free Software Foundation, Inc.
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

#include <gr_peak_detector2_fb.h>
#include <gr_io_signature.h>
#include <string.h>

gr_peak_detector2_fb_sptr
gr_make_peak_detector2_fb (float threshold_factor_rise,
			   int look_ahead, float alpha)
{
  return gnuradio::get_initial_sptr(new gr_peak_detector2_fb (threshold_factor_rise, 
				  look_ahead, alpha));
}

gr_peak_detector2_fb::gr_peak_detector2_fb (float threshold_factor_rise, 
					    int look_ahead, float alpha)
  : gr_sync_block ("peak_detector2_fb",
		   gr_make_io_signature (1, 1, sizeof(float)),
		   gr_make_io_signature2 (1, 2, sizeof(char), sizeof(float))),
    d_threshold_factor_rise(threshold_factor_rise), 
    d_look_ahead(look_ahead), d_alpha(alpha), d_avg(0.0f), d_found(false)
{
}

int
gr_peak_detector2_fb::work (int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items) {
  float *iptr = (float *) input_items[0];
  char *optr = (char *) output_items[0];
  
  assert(noutput_items >= 2);

  memset(optr, 0, noutput_items*sizeof(char));

  for (int i = 0; i < noutput_items; i++) {

    if (!d_found) {
      // Have not yet detected presence of peak
      if (iptr[i] > d_avg * (1.0f + d_threshold_factor_rise)) {
	d_found = true;
	d_look_ahead_remaining = d_look_ahead;
        d_peak_val = -(float)INFINITY;
      } 
      else {
	d_avg = d_alpha*iptr[i] + (1.0f - d_alpha)*d_avg;
      }
    } 
    else {
      // Detected presence of peak
      if (iptr[i] > d_peak_val) {
        d_peak_val = iptr[i];
        d_peak_ind = i;
      } 
      else if (d_look_ahead_remaining <= 0) {
        optr[d_peak_ind] = 1;
        d_found = false;
        d_avg = iptr[i];
      }
      
      // Have not yet located peak, loop and keep searching.
      d_look_ahead_remaining--;
    }
    
    // Every iteration of the loop, write debugging signal out if
    // connected:
    if (output_items.size() == 2) {
      float *sigout = (float *) output_items[1];
      sigout[i] = d_avg;
    }
  } // loop
  
  if (!d_found) 
    return noutput_items;
  
  // else if detected presence, keep searching during the next call to work.
  int tmp = d_peak_ind;
  d_peak_ind = 1;
  
  return tmp - 1; 
}


