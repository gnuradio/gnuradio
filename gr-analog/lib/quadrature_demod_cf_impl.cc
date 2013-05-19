/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2010,2012 Free Software Foundation, Inc.
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

#include "quadrature_demod_cf_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

namespace gr {
  namespace analog {
    
    quadrature_demod_cf::sptr
    quadrature_demod_cf::make(float gain)
    {
      return gnuradio::get_initial_sptr
	(new quadrature_demod_cf_impl(gain));
    }

    quadrature_demod_cf_impl::quadrature_demod_cf_impl(float gain)
      : sync_block("quadrature_demod_cf",
		      io_signature::make(1, 1, sizeof(gr_complex)),
		      io_signature::make(1, 1, sizeof(float))),
	d_gain(gain)
    {
      set_history(2); // we need to look at the previous value
    }

    quadrature_demod_cf_impl::~quadrature_demod_cf_impl()
    {
    }

    int
    quadrature_demod_cf_impl::work(int noutput_items,
				   gr_vector_const_void_star &input_items,
				   gr_vector_void_star &output_items)
    {
      gr_complex *in = (gr_complex*)input_items[0];
      float *out = (float*)output_items[0];
      in++;	// ensure that in[-1] is valid

      for(int i = 0; i < noutput_items; i++) {
	gr_complex product = in[i] * conj(in[i-1]);
	out[i] = d_gain * gr::fast_atan2f(imag(product), real(product));
      }

      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */
