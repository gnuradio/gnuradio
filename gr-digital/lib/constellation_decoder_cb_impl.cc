/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#include "constellation_decoder_cb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace digital {

    constellation_decoder_cb::sptr
    constellation_decoder_cb::make(constellation_sptr constellation)
    {
      return gnuradio::get_initial_sptr
	(new constellation_decoder_cb_impl(constellation));
    }

    constellation_decoder_cb_impl::
    constellation_decoder_cb_impl(constellation_sptr constellation)
      : block("constellation_decoder_cb",
		 io_signature::make(1, 1, sizeof(gr_complex)),
		 io_signature::make(1, 1, sizeof(unsigned char))),
	d_constellation(constellation),
	d_dim(constellation->dimensionality())
    {
      set_relative_rate(1.0 / ((double)d_dim));
    }

    constellation_decoder_cb_impl::~constellation_decoder_cb_impl()
    {
    }

    void
    constellation_decoder_cb_impl::forecast(int noutput_items,
					    gr_vector_int &ninput_items_required)
    {
      unsigned int input_required = noutput_items * d_dim;

      unsigned ninputs = ninput_items_required.size();
      for(unsigned int i = 0; i < ninputs; i++)
	ninput_items_required[i] = input_required;
    }

    int
    constellation_decoder_cb_impl::general_work(int noutput_items,
						gr_vector_int &ninput_items,
						gr_vector_const_void_star &input_items,
						gr_vector_void_star &output_items)
    {
      gr_complex const *in = (const gr_complex*)input_items[0];
      unsigned char *out = (unsigned char*)output_items[0];

      for(int i = 0; i < noutput_items; i++) {
	out[i] = d_constellation->decision_maker(&(in[i*d_dim]));
      }

      consume_each(noutput_items * d_dim);
      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
