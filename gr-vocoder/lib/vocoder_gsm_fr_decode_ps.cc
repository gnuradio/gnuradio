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
#include "vocoder_gsm_fr_decode_ps.h"
extern "C"{
#include "gsm/gsm.h"
}
#include <gr_io_signature.h>
#include <stdexcept>
#include <assert.h>

vocoder_gsm_fr_decode_ps_sptr
vocoder_make_gsm_fr_decode_ps ()
{
  return gnuradio::get_initial_sptr(new vocoder_gsm_fr_decode_ps ());
}

vocoder_gsm_fr_decode_ps::vocoder_gsm_fr_decode_ps ()
  : gr_sync_interpolator ("vocoder_gsm_fr_decode_ps",
			  gr_make_io_signature (1, 1, sizeof (gsm_frame)),
			  gr_make_io_signature (1, 1, sizeof (short)),
			  GSM_SAMPLES_PER_FRAME)
{
  if ((d_gsm = gsm_create ()) == 0)
    throw std::runtime_error ("vocoder_gsm_fr_decode_ps: gsm_create failed");
}

vocoder_gsm_fr_decode_ps::~vocoder_gsm_fr_decode_ps ()
{
  gsm_destroy (d_gsm);
}

int
vocoder_gsm_fr_decode_ps::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *) input_items[0];
  short *out = (short *) output_items[0];

  assert ((noutput_items % GSM_SAMPLES_PER_FRAME) == 0);

  for (int i = 0; i < noutput_items; i += GSM_SAMPLES_PER_FRAME){
    gsm_decode (d_gsm, const_cast<unsigned char*>(in), out);
    in += sizeof (gsm_frame);
    out += GSM_SAMPLES_PER_FRAME;
  }

  return noutput_items;
}
