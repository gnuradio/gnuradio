/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2013 Free Software Foundation, Inc.
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

#include "gsm_fr_decode_ps_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <assert.h>

namespace gr {
  namespace vocoder {

    gsm_fr_decode_ps::sptr
    gsm_fr_decode_ps::make()
    {
      return gnuradio::get_initial_sptr
	(new gsm_fr_decode_ps_impl());
    }

    gsm_fr_decode_ps_impl::gsm_fr_decode_ps_impl()
      : sync_interpolator("vocoder_gsm_fr_decode_ps",
			     io_signature::make(1, 1, sizeof(gsm_frame)),
			     io_signature::make(1, 1, sizeof(short)),
			     GSM_SAMPLES_PER_FRAME)
    {
      if((d_gsm = gsm_create()) == 0)
	throw std::runtime_error("gsm_fr_decode_ps_impl: gsm_create failed");
    }

    gsm_fr_decode_ps_impl::~gsm_fr_decode_ps_impl()
    {
      gsm_destroy(d_gsm);
    }

    int
    gsm_fr_decode_ps_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char*)input_items[0];
      short *out = (short*)output_items[0];

      assert((noutput_items % GSM_SAMPLES_PER_FRAME) == 0);

      for(int i = 0; i < noutput_items; i += GSM_SAMPLES_PER_FRAME) {
	gsm_decode(d_gsm, const_cast<unsigned char*>(in), out);
	in += sizeof(gsm_frame);
	out += GSM_SAMPLES_PER_FRAME;
      }

      return noutput_items;
    }

  } /* namespace vocoder */
} /* namespace gr */
