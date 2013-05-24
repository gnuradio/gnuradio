/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2010,2012 Free Software Foundation, Inc.
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

#include "agc2_ff_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace analog {

    agc2_ff::sptr
    agc2_ff::make(float attack_rate, float decay_rate,
		  float reference, float gain)
    {
      return gnuradio::get_initial_sptr
	(new agc2_ff_impl(attack_rate, decay_rate,
			  reference, gain));
    }

    agc2_ff_impl::~agc2_ff_impl()
    {
    }

    agc2_ff_impl::agc2_ff_impl(float attack_rate, float decay_rate,
			       float reference, float gain)
    : sync_block("agc2_ff",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float))),
      kernel::agc2_ff(attack_rate, decay_rate,
                      reference, gain, 65536)
    {
    }

    int
    agc2_ff_impl::work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
      const float *in = (const float*)input_items[0];
      float *out = (float*)output_items[0];
      scaleN(out, in, noutput_items);
      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */
