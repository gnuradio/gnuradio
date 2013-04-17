/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include "nlog10_ff_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    nlog10_ff::sptr nlog10_ff::make(float n, size_t vlen, float k)
    {
      return gnuradio::get_initial_sptr(new nlog10_ff_impl(n, vlen, k));
    }

    nlog10_ff_impl::nlog10_ff_impl(float n, size_t vlen, float k)
      : sync_block("nlog10_ff",
		      io_signature::make (1, 1, sizeof(float)*vlen),
		      io_signature::make (1, 1, sizeof(float)*vlen)),
	d_n(n), d_vlen(vlen), d_k(k)
    {
    }

    int
    nlog10_ff_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];
      int noi = noutput_items * d_vlen;
      float n = d_n;
      float k = d_k;

      for (int i = 0; i < noi; i++)
	out[i] = n * log10(std::max(in[i], (float) 1e-18)) + k;

      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
