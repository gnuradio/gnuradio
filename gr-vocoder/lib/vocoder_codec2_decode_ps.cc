/* -*- c++ -*- */
/*
 * Copyright 2005,2011 Free Software Foundation, Inc.
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

#include "vocoder_codec2_decode_ps.h"

extern "C" {
#include "codec2/codec2.h"
}

#include <gr_io_signature.h>
#include <stdexcept>
#include <assert.h>

vocoder_codec2_decode_ps_sptr
vocoder_make_codec2_decode_ps ()
{
  return gnuradio::get_initial_sptr(new vocoder_codec2_decode_ps ());
}

vocoder_codec2_decode_ps::vocoder_codec2_decode_ps ()
  : gr_sync_interpolator ("vocoder_codec2_decode_ps",
			  gr_make_io_signature (1, 1, CODEC2_BITS_PER_FRAME * sizeof (char)),
			  gr_make_io_signature (1, 1, sizeof (short)),
			  CODEC2_SAMPLES_PER_FRAME)
{
  if ((d_codec2 = codec2_create ()) == 0)
    throw std::runtime_error ("vocoder_codec2_decode_ps: codec2_create failed");
}

vocoder_codec2_decode_ps::~vocoder_codec2_decode_ps ()
{
  codec2_destroy(d_codec2);
}

int
vocoder_codec2_decode_ps::work (int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *) input_items[0];
  short *out = (short *) output_items[0];

  assert ((noutput_items % CODEC2_SAMPLES_PER_FRAME) == 0);

  for (int i = 0; i < noutput_items; i += CODEC2_SAMPLES_PER_FRAME){
    codec2_decode (d_codec2, out, const_cast<unsigned char*>(in));
    in += CODEC2_BITS_PER_FRAME * sizeof (char);
    out += CODEC2_SAMPLES_PER_FRAME;
  }

  return noutput_items;
}
