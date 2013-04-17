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

#include "codec2_encode_sp_impl.h"

extern "C" {
#include "codec2/codec2.h"
}

#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
  namespace vocoder {

    codec2_encode_sp::sptr
    codec2_encode_sp::make()
    {
      return gnuradio::get_initial_sptr
	(new codec2_encode_sp_impl());
    }

    codec2_encode_sp_impl::codec2_encode_sp_impl()
      : sync_decimator("vocoder_codec2_encode_sp",
			  io_signature::make(1, 1, sizeof(short)),
			  io_signature::make(1, 1, CODEC2_BITS_PER_FRAME * sizeof(char)),
			  CODEC2_SAMPLES_PER_FRAME)
    {
      if((d_codec2 = codec2_create()) == 0)
	throw std::runtime_error("codec2_encode_sp_impl: codec2_create failed");
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
	codec2_encode(d_codec2, out, const_cast<short*>(in));
	in += CODEC2_SAMPLES_PER_FRAME;
	out += CODEC2_BITS_PER_FRAME * sizeof(char);
      }

      return noutput_items;
    }

  } /* namespace vocoder */
} /* namespace gr */
