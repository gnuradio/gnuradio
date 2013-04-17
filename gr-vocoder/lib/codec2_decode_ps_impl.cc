/* -*- c++ -*- */
/*
 * Copyright 2005,2011,2013 Free Software Foundation, Inc.
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

#include "codec2_decode_ps_impl.h"

extern "C" {
#include "codec2/codec2.h"
}

#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <assert.h>

namespace gr {
  namespace vocoder {

    codec2_decode_ps::sptr
    codec2_decode_ps::make()
    {
      return gnuradio::get_initial_sptr
	(new codec2_decode_ps_impl());
    }


    codec2_decode_ps_impl::codec2_decode_ps_impl ()
      : sync_interpolator("vocoder_codec2_decode_ps",
			     io_signature::make(1, 1, CODEC2_BITS_PER_FRAME * sizeof(char)),
			     io_signature::make (1, 1, sizeof(short)),
			     CODEC2_SAMPLES_PER_FRAME)
    {
      if((d_codec2 = codec2_create()) == 0)
	throw std::runtime_error("codec2_decode_ps_impl: codec2_create failed");
    }

    codec2_decode_ps_impl::~codec2_decode_ps_impl()
    {
      codec2_destroy(d_codec2);
    }

    int
    codec2_decode_ps_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char*)input_items[0];
      short *out = (short *) output_items[0];

      assert((noutput_items % CODEC2_SAMPLES_PER_FRAME) == 0);

      for(int i = 0; i < noutput_items; i += CODEC2_SAMPLES_PER_FRAME) {
	codec2_decode (d_codec2, out, const_cast<unsigned char*>(in));
	in += CODEC2_BITS_PER_FRAME * sizeof (char);
	out += CODEC2_SAMPLES_PER_FRAME;
      }

      return noutput_items;
    }

  } /* namespace vocoder */
} /* namespace gr */
