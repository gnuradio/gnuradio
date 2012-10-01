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
#include <gr_io_signature.h>

namespace gr {
  namespace blocks {

    interleaved_short_to_complex::sptr interleaved_short_to_complex::make()
    {
      return gnuradio::get_initial_sptr(new interleaved_short_to_complex_impl());
    }

    interleaved_short_to_complex_impl::interleaved_short_to_complex_impl()
      : gr_sync_decimator("interleaved_short_to_complex",
			  gr_make_io_signature (1, 1, sizeof(short)),
			  gr_make_io_signature (1, 1, sizeof(gr_complex)),
			  2)
    {
    }

    int
    interleaved_short_to_complex_impl::work(int noutput_items,
					    gr_vector_const_void_star &input_items,
					    gr_vector_void_star &output_items)
    {
      const short *in = (const short *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      
      interleaved_short_array_to_complex (in, out, 2 * noutput_items);
      
      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
