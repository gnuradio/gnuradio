/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

#include <gr_lms_dfe_ff.h>
#include <gr_io_signature.h>
#include <gr_misc.h>
#include <iostream>

float
slice(float val)
{
  if (val>0)
    return 1;
  else
    return -1;
}

gr_lms_dfe_ff_sptr
gr_make_lms_dfe_ff (float lambda_ff, float lambda_fb, 
		    unsigned int num_fftaps, unsigned int num_fbtaps)
{
  return gr_lms_dfe_ff_sptr (new gr_lms_dfe_ff (lambda_ff,lambda_fb,num_fftaps,num_fbtaps));
}

gr_lms_dfe_ff::gr_lms_dfe_ff (float lambda_ff, float lambda_fb , 
			      unsigned int num_fftaps, unsigned int num_fbtaps)
  : gr_sync_block ("lms_dfe_ff",
		   gr_make_io_signature (1, 1, sizeof (float)),
		   gr_make_io_signature (1, 1, sizeof (float))),
    d_lambda_ff (lambda_ff), d_lambda_fb (lambda_fb), 
    d_ff_delayline(gr_rounduppow2(num_fftaps)),
    d_fb_delayline(gr_rounduppow2(num_fbtaps)),
    d_ff_taps(num_fftaps), d_fb_taps(num_fbtaps),
    d_ff_index(0), d_fb_index(0)
{
  gr_zero_vector(d_ff_taps);
  d_ff_taps [d_ff_taps.size()/2] = 1;

  gr_zero_vector(d_fb_taps);
  gr_zero_vector(d_ff_delayline);
  gr_zero_vector(d_fb_delayline);
}

int
gr_lms_dfe_ff::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  const float *iptr = (const float *) input_items[0];
  float *optr = (float *) output_items[0];
  
  float acc, decision, error;
  unsigned int i;

  unsigned int ff_mask = d_ff_delayline.size() - 1;	// size is power of 2
  unsigned int fb_mask = d_fb_delayline.size() - 1;

  int	size = noutput_items;
  while(size-- > 0) {
    acc = 0; 
    d_ff_delayline[d_ff_index] = *iptr++;

    // Compute output
    for (i=0; i < d_ff_taps.size(); i++) 
      acc += d_ff_delayline[(i+d_ff_index) & ff_mask] * d_ff_taps[i];
    
    for (i=0; i < d_fb_taps.size(); i++)
      acc -= d_fb_delayline[(i+d_fb_index) & fb_mask] * d_fb_taps[i];

    decision = slice(acc);
    error = decision - acc;
    
    // Update taps
    for (i=0; i < d_ff_taps.size(); i++)
      d_ff_taps[i] += d_lambda_ff * error * d_ff_delayline[(i+d_ff_index) & ff_mask];
    
    for (i=0; i < d_fb_taps.size(); i++)
      d_fb_taps[i] -= d_lambda_fb * error * d_fb_delayline[(i+d_fb_index) & fb_mask];
    
    d_fb_index = (d_fb_index - 1) & fb_mask;   	// Decrement index
    d_ff_index = (d_ff_index - 1) & ff_mask;   	// Decrement index

    d_fb_delayline[d_fb_index] = decision; 	// Save decision in feedback

    *optr++ = acc;   // Output decision
  }

  if (0){
    std::cout << "FF Taps\t";
    for(i=0;i<d_ff_taps.size();i++)
      std::cout << d_ff_taps[i] << "\t";
    std::cout << std::endl << "FB Taps\t";
    for(i=0;i<d_fb_taps.size();i++)
      std::cout << d_fb_taps[i] << "\t";
    std::cout << std::endl;
  }

  return noutput_items;
}
