/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#include "complex_to_interleaved_char_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    complex_to_interleaved_char::sptr complex_to_interleaved_char::make(bool vector)
    {
      return gnuradio::get_initial_sptr(new complex_to_interleaved_char_impl(vector));
    }

    complex_to_interleaved_char_impl::complex_to_interleaved_char_impl(bool vector)
      : sync_interpolator("complex_to_interleaved_char",
			     io_signature::make (1, 1, sizeof(gr_complex)),
			     io_signature::make (1, 1, vector?2*sizeof(char):sizeof(char)),
			     vector?1:2),
        d_vector(vector)
    {
    }

    int
    complex_to_interleaved_char_impl::work(int noutput_items,
					    gr_vector_const_void_star &input_items,
					    gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      char *out = (char *) output_items[0];

      int npairs = (d_vector?noutput_items:noutput_items/2);
      for (int i = 0; i < npairs; i++){
        *out++ = (char) lrintf(in[i].real());	// FIXME saturate?
        *out++ = (char) lrintf(in[i].imag());
      }

      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
