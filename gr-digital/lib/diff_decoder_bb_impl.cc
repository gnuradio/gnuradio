/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2012 Free Software Foundation, Inc.
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

#include "diff_decoder_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace digital {
    diff_decoder_bb::sptr
    diff_decoder_bb::make(unsigned int modulus)
    {
      return gnuradio::get_initial_sptr
	(new diff_decoder_bb_impl(modulus));
    }

    diff_decoder_bb_impl::diff_decoder_bb_impl(unsigned int modulus)
      : sync_block("diff_decoder_bb",
		      io_signature::make(1, 1, sizeof(unsigned char)),
		      io_signature::make(1, 1, sizeof(unsigned char))),
	d_modulus(modulus)
    {
      set_history(2);	// need to look at two inputs
    }

    diff_decoder_bb_impl::~diff_decoder_bb_impl()
    {
    }

    int
    diff_decoder_bb_impl::work(int noutput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char*)input_items[0];
      unsigned char *out = (unsigned char*)output_items[0];
      in += 1;	// ensure that in[-1] is valid

      unsigned modulus = d_modulus;

      for(int i = 0; i < noutput_items; i++) {
	out[i] = (in[i] - in[i-1]) % modulus;
      }

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
