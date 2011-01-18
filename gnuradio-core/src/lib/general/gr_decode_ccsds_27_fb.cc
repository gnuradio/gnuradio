/* -*- c++ -*- */
/*
 * Copyright 2008,2010 Free Software Foundation, Inc.
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

#include <gr_decode_ccsds_27_fb.h>
#include <gr_io_signature.h>

gr_decode_ccsds_27_fb_sptr 
gr_make_decode_ccsds_27_fb()
{
  return gnuradio::get_initial_sptr(new gr_decode_ccsds_27_fb());
}

gr_decode_ccsds_27_fb::gr_decode_ccsds_27_fb()
  : gr_sync_decimator("decode_ccsds_27_fb",
		      gr_make_io_signature(1, 1, sizeof(float)),
		      gr_make_io_signature(1, 1, sizeof(char)),
		      2*8)  // Rate 1/2 code, unpacked to packed translation
{
    float RATE = 0.5;
    float ebn0 = 12.0;
    float esn0 = RATE*pow(10.0, ebn0/10.0);

    gen_met(d_mettab, 100, esn0, 0.0, 256);
    viterbi_chunks_init(d_state0);
}

gr_decode_ccsds_27_fb::~gr_decode_ccsds_27_fb()
{
}

int 
gr_decode_ccsds_27_fb::work(int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
{
  const float *in = (const float *)input_items[0];
  unsigned char *out = (unsigned char *)output_items[0];

  for (int i = 0; i < noutput_items*16; i++) {
    // Translate and clip [-1.0..1.0] to [28..228]
    float sample = in[i]*100.0+128.0;
    if (sample > 255.0)
	sample = 255.0;
    else if (sample < 0.0)
	sample = 0.0;
    unsigned char sym = (unsigned char)(floor(sample));
    
    d_viterbi_in[d_count % 4] = sym;
    if ((d_count % 4) == 3) {
      // Every fourth symbol, perform butterfly operation
      viterbi_butterfly2(d_viterbi_in, d_mettab, d_state0, d_state1);
      
      // Every sixteenth symbol, read out a byte
      if (d_count % 16 == 11) {
	// long metric = 
	viterbi_get_output(d_state0, out++);
	// printf("%li\n", *(out-1), metric);
      }
    }
    
    d_count++;
  }

  return noutput_items;
}
