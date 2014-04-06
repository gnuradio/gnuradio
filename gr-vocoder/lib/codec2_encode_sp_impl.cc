/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2014 Free Software Foundation, Inc.
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

extern "C" {
#include "codec2/codec2.h"
}

#include "codec2_encode_sp_impl.h"

#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace gr {
  namespace vocoder {

    codec2_encode_sp::sptr
    codec2_encode_sp::make(int mode)
    {
      CODEC2 *codec2;
      int samples_per_frame, bits_per_frame;

      // Check the number of input samples and output bits per frame.
      codec2 = codec2_create(mode);
      samples_per_frame = codec2_samples_per_frame(codec2);
      bits_per_frame = codec2_bits_per_frame(codec2);
      codec2_destroy(codec2);

      return gnuradio::get_initial_sptr
	(new codec2_encode_sp_impl(mode, samples_per_frame, bits_per_frame));
    }

    codec2_encode_sp_impl::codec2_encode_sp_impl(int mode, int samples_per_frame, int bits_per_frame)
      : sync_decimator("vocoder_codec2_encode_sp",
			  io_signature::make(1, 1, sizeof(short)),
			  io_signature::make(1, 1, bits_per_frame * sizeof(char)),
			  samples_per_frame),
      d_frame_buf((bits_per_frame + 7) / 8, 0)
    {
      if((d_codec2 = codec2_create(mode)) == 0)
	throw std::runtime_error("codec2_encode_sp_impl: codec2_create failed");
      d_samples_per_frame = samples_per_frame;
      d_bits_per_frame = bits_per_frame;
    }

    codec2_encode_sp_impl::~codec2_encode_sp_impl()
    {
      codec2_destroy(d_codec2);
    }

    int
    codec2_encode_sp_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const short *in = (const short*)input_items[0];
      unsigned char *out = (unsigned char*)output_items[0];

      for(int i = 0; i < noutput_items; i++) {
	codec2_encode(d_codec2, &d_frame_buf[0], const_cast<short*>(in));
	unpack_frame((const unsigned char *) &d_frame_buf[0], out);
	in += d_samples_per_frame;
	out += d_bits_per_frame * sizeof(char);
      }

      return noutput_items;
    }

    void
    codec2_encode_sp_impl::unpack_frame(const unsigned char *packed, unsigned char *out)
    {
      int byte_idx = 0, bit_idx = 0;
      for(int k = 0; k < d_bits_per_frame; k++) {
	out[k] = (packed[byte_idx] >> (7-bit_idx)) & 0x01;
	bit_idx = (bit_idx + 1) % 8;
	if (bit_idx == 0) {
	  byte_idx++;
	}
      }
    }

  } /* namespace vocoder */
} /* namespace gr */
