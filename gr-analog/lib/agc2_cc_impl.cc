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

#include "agc2_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace analog {

    agc2_cc::sptr
    agc2_cc::make(float attack_rate, float decay_rate,
		  float reference, float gain)
    {
      return gnuradio::get_initial_sptr
	(new agc2_cc_impl(attack_rate, decay_rate,
			  reference, gain));
    }

    agc2_cc_impl::agc2_cc_impl(float attack_rate, float decay_rate,
			       float reference, float gain)
      : sync_block("agc2_cc",
                   io_signature::make(1, 1, sizeof(gr_complex)),
                   io_signature::make(1, 1, sizeof(gr_complex))),
	kernel::agc2_cc(attack_rate, decay_rate,
			reference, gain, 65536)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1, alignment_multiple));
    }

    agc2_cc_impl::~agc2_cc_impl()
    {
    }

    int
    agc2_cc_impl::work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];
      scaleN(out, in, noutput_items);
      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */
