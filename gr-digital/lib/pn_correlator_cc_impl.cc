/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2012 Free Software Foundation, Inc.
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

#include "pn_correlator_cc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace digital {

    pn_correlator_cc::sptr
    pn_correlator_cc::make(int degree, int mask, int seed)
    {
      return gnuradio::get_initial_sptr
	(new pn_correlator_cc_impl(degree, mask, seed));
    }

    pn_correlator_cc_impl::pn_correlator_cc_impl(int degree,
						 int mask,
						 int seed)
      : sync_decimator("pn_correlator_cc",
			  io_signature::make(1, 1, sizeof(gr_complex)),
			  io_signature::make(1, 1, sizeof(gr_complex)),
			  (unsigned int)((1ULL << degree)-1)), // PN code length
      d_pn(0.0f)
    {
      d_len = (unsigned int)((1ULL << degree)-1);
      if(mask == 0)
	mask = glfsr::glfsr_mask(degree);
      d_reference = new glfsr(mask, seed);
      for(int i = 0; i < d_len; i++)	// initialize to last value in sequence
	d_pn = 2.0*d_reference->next_bit()-1.0;
    }

    pn_correlator_cc_impl::~pn_correlator_cc_impl()
    {
      delete d_reference;
    }

    int
    pn_correlator_cc_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];
      gr_complex sum;

      for(int i = 0; i < noutput_items; i++) {
	sum = 0.0;

	for(int j = 0; j < d_len; j++) {
	  if(j != 0)		            // retard PN generator one sample per period
	    d_pn = 2.0*d_reference->next_bit()-1.0; // no conditionals
	  sum += *in++ * d_pn;
	}

	*out++ = sum*gr_complex(1.0/d_len, 0.0);
      }

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
