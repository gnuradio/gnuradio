/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#include <gr_ofdm_sampler.h>
#include <gr_io_signature.h>
#include <gr_expj.h>

gr_ofdm_sampler_sptr
gr_make_ofdm_sampler (unsigned int fft_length, 
		      unsigned int symbol_length)
{
  return gr_ofdm_sampler_sptr (new gr_ofdm_sampler (fft_length, symbol_length));
}

gr_ofdm_sampler::gr_ofdm_sampler (unsigned int fft_length, 
				  unsigned int symbol_length)
  : gr_block ("ofdm_sampler",
	      gr_make_io_signature2 (2, 2, sizeof (gr_complex), sizeof(char)),
	      gr_make_io_signature (1, 1, sizeof (gr_complex)*fft_length)),
    d_fft_length(fft_length), d_symbol_length(symbol_length)
{
}

void
gr_ofdm_sampler::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  // FIXME do we need more
  int nreqd  = (noutput_items-1) * d_symbol_length + d_fft_length;
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = nreqd;
}

int
gr_ofdm_sampler::general_work (int noutput_items,
			       gr_vector_int &ninput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
{
  gr_complex *iptr = (gr_complex *) input_items[0];
  char *trigger = (char *) input_items[1];

  gr_complex *optr = (gr_complex *) output_items[0];

  int found=0;

  unsigned int i=d_fft_length-1;

  while(!found && i<std::min(ninput_items[0],ninput_items[1]) )
    if(trigger[i])
      found = 1;
    else
      i++;

  if(found) {
    assert(i-d_fft_length+1 >= 0);
    for(unsigned int j=i-d_fft_length+1;j<=i;j++)
      *optr++ = iptr[j];
    consume_each(i-d_fft_length+2);
    //printf("OFDM Sampler found:  ninput_items: %d/%d   noutput_items: %d  consumed: %d   found: %d\n", 
    //   ninput_items[0], ninput_items[1], noutput_items, (i-d_fft_length+2), found);
  }
  else {
    consume_each(i-d_fft_length+1);
    //printf("OFDM Sampler not found:  ninput_items: %d/%d   noutput_items: %d  consumed: %d   found: %d\n", 
    //  ninput_items[0], ninput_items[1], noutput_items, (i-d_fft_length+1), found);
 }


  return found;
}
