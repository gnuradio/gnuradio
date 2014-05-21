/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include "decode_ccsds_27_fb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace fec {

    decode_ccsds_27_fb::sptr decode_ccsds_27_fb::make()
    {
      return gnuradio::get_initial_sptr(new decode_ccsds_27_fb_impl());
    }

    decode_ccsds_27_fb_impl::decode_ccsds_27_fb_impl()
      : sync_decimator("decode_ccsds_27_fb",
                       io_signature::make (1, 1, sizeof(float)),
                       io_signature::make (1, 1, sizeof(char)),
                       2*8), d_count(0)  // Rate 1/2 code, unpacked to packed conversion
    {
      float RATE = 0.5;
      float ebn0 = 12.0;
      float esn0 = RATE*pow(10.0, ebn0/10.0);

      gen_met(d_mettab, 100, esn0, 0.0, 256);
      viterbi_chunks_init(d_state0);
      viterbi_chunks_init(d_state1);
    }

    int
    decode_ccsds_27_fb_impl::work(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
    {
      const float *in = (const float*)input_items[0];
      unsigned char *out = (unsigned char*)output_items[0];

      for (int i = 0; i < noutput_items*16; i++) {
	// Translate and clip [-1.0..1.0] to [28..228]
	float sample = in[i]*100.0+128.0;
	if(sample > 255.0)
	  sample = 255.0;
	else if(sample < 0.0)
	  sample = 0.0;
	unsigned char sym = (unsigned char)(floor(sample));

	d_viterbi_in[d_count % 4] = sym;
	if((d_count % 4) == 3) {
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

  } /* namespace fec */
}/* namespace gr */
