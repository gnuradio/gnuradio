/* -*- c++ -*- */
/*
 * Copyright 2005,2010 Free Software Foundation, Inc.
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

#include <gr_fake_channel_coder_pp.h>
#include <gr_io_signature.h>
#include <string.h>
#include <stdexcept>
#include <string.h>

static const int PAD_VAL = 0xAA;

gr_fake_channel_encoder_pp_sptr
gr_make_fake_channel_encoder_pp(int input_vlen, int output_vlen)
{
  return gnuradio::get_initial_sptr(new gr_fake_channel_encoder_pp(input_vlen,
									output_vlen));
}

gr_fake_channel_encoder_pp::gr_fake_channel_encoder_pp(int input_vlen, int output_vlen)
  : gr_sync_block("fake_channel_encoder_pp",
		  gr_make_io_signature(1, 1, input_vlen * sizeof(unsigned char)),
		  gr_make_io_signature(1, 1, output_vlen * sizeof(unsigned char))),
    d_input_vlen(input_vlen), d_output_vlen(output_vlen)
{
  if (input_vlen <= 0 || output_vlen <= 0 || input_vlen > output_vlen)
    throw std::invalid_argument("gr_fake_channel_encoder_pp");
}

gr_fake_channel_encoder_pp::~gr_fake_channel_encoder_pp()
{
}

int
gr_fake_channel_encoder_pp::work (int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];
  int	npad = d_output_vlen - d_input_vlen;
  
  for (int i = 0; i < noutput_items; i++){
    memcpy(out, in, d_input_vlen);
    memset(out + d_input_vlen, PAD_VAL, npad);
    in += d_input_vlen;
    out += d_output_vlen;
  }

  return noutput_items;
}

// ------------------------------------------------------------------------

gr_fake_channel_decoder_pp_sptr
gr_make_fake_channel_decoder_pp(int input_vlen, int output_vlen)
{
  return gnuradio::get_initial_sptr(new gr_fake_channel_decoder_pp(input_vlen,
									output_vlen));
}

gr_fake_channel_decoder_pp::gr_fake_channel_decoder_pp(int input_vlen, int output_vlen)
  : gr_sync_block("fake_channel_decoder_pp",
		  gr_make_io_signature(1, 1, input_vlen * sizeof(unsigned char)),
		  gr_make_io_signature(1, 1, output_vlen * sizeof(unsigned char))),
    d_input_vlen(input_vlen), d_output_vlen(output_vlen)
{
  if (input_vlen <= 0 || output_vlen <= 0 || output_vlen > input_vlen)
    throw std::invalid_argument("gr_fake_channel_decoder_pp");
}

gr_fake_channel_decoder_pp::~gr_fake_channel_decoder_pp()
{
}

int
gr_fake_channel_decoder_pp::work (int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];
  
  for (int i = 0; i < noutput_items; i++){
    memcpy(out, in, d_output_vlen);
    in += d_input_vlen;
    out += d_output_vlen;
  }

  return noutput_items;
}
