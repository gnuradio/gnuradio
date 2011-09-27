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

#include <vocoder_g721_decode_bs.h>
#include <gr_io_signature.h>
#include <limits.h>

extern "C" {
#include "g7xx/g72x.h"
}

class vocoder_g721_decode_bs_impl : public vocoder_g721_decode_bs
{
public:

  vocoder_g721_decode_bs_impl();
  ~vocoder_g721_decode_bs_impl();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

private:

  g72x_state d_state;

};

vocoder_g721_decode_bs_sptr vocoder_make_g721_decode_bs()
{
  return gnuradio::get_initial_sptr(new vocoder_g721_decode_bs_impl());
}

vocoder_g721_decode_bs_impl::vocoder_g721_decode_bs_impl()
  : gr_sync_block("vocoder_g721_decode_bs",
		  gr_make_io_signature (1, 1, sizeof (unsigned char)),
		  gr_make_io_signature (1, 1, sizeof (short)))
{
  g72x_init_state(&d_state);
}

vocoder_g721_decode_bs_impl::~vocoder_g721_decode_bs_impl()
{
}

int 
vocoder_g721_decode_bs_impl::work(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *)input_items[0];
  short *out = (short *)output_items[0];

  for(int i = 0; i < noutput_items; i++)
    out[i] = g721_decoder(in[i], AUDIO_ENCODING_LINEAR, &d_state);
  
  return noutput_items;
}
