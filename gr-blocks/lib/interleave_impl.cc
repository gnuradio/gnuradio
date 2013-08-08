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

#include "interleave_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    interleave::sptr interleave::make(size_t itemsize)
    {
      return gnuradio::get_initial_sptr(new interleave_impl(itemsize));
    }

    interleave_impl::interleave_impl(size_t itemsize)
      : sync_interpolator("interleave",
			     io_signature::make (1, io_signature::IO_INFINITE, itemsize),
			     io_signature::make (1, 1, itemsize),
			     1),
	d_itemsize(itemsize)
    {
    }

    bool
    interleave_impl::check_topology(int ninputs, int noutputs)
    {
      set_interpolation(ninputs);
      return true;
    }

    int
    interleave_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      size_t nchan = input_items.size();
      size_t itemsize = d_itemsize;
      const char **in = (const char **)&input_items[0];
      char *out = (char *)output_items[0];
      
      for (int i = 0; i < noutput_items; i += nchan) {
	for (unsigned int n = 0; n < nchan; n++) {
	  memcpy (out, in[n], itemsize);
	  out += itemsize;
	  in[n] += itemsize;
	}
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
