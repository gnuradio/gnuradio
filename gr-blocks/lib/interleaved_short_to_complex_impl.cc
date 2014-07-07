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

#include "interleaved_short_to_complex_impl.h"
#include "interleaved_short_array_to_complex.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    interleaved_short_to_complex::sptr interleaved_short_to_complex::make(bool vector_input, bool swap)
    {
      return gnuradio::get_initial_sptr(new interleaved_short_to_complex_impl(vector_input, swap));
    }

    interleaved_short_to_complex_impl::interleaved_short_to_complex_impl(bool vector_input, bool swap)
      : sync_decimator("interleaved_short_to_complex",
		       gr::io_signature::make (1, 1, (vector_input?2:1)*sizeof(short)),
		       gr::io_signature::make (1, 1, sizeof(gr_complex)),
		       vector_input?1:2),
        d_vector_input(vector_input), d_swap(swap)
    {
    }

    void interleaved_short_to_complex_impl::set_swap(bool swap)
    {
		d_swap = swap;
	}

    int
    interleaved_short_to_complex_impl::work(int noutput_items,
					    gr_vector_const_void_star &input_items,
					    gr_vector_void_star &output_items)
    {
      const short *in = (const short *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      interleaved_short_array_to_complex (in, out, 2 * noutput_items);

      if (d_swap) {
        float* p = (float*)output_items[0];
        for (int i = 0; i < noutput_items; ++i) {
          float f = p[2*i+1];
          p[2*i+1] = p[2*i+0];
          p[2*i+0] = f;
        }
      }

      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
