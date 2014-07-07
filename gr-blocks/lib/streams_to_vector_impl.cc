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

#include "streams_to_vector_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    streams_to_vector::sptr streams_to_vector::make(size_t itemsize, size_t nstreams)
    {
      return gnuradio::get_initial_sptr(new streams_to_vector_impl(itemsize, nstreams));
    }

    streams_to_vector_impl::streams_to_vector_impl(size_t itemsize, size_t nstreams)
      : sync_block ("streams_to_vector",
		       io_signature::make (nstreams, nstreams, itemsize),
		       io_signature::make (1, 1, nstreams * itemsize))
    {
    }

    int
    streams_to_vector_impl::work(int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
    {
      size_t itemsize = input_signature()->sizeof_stream_item(0);
      int nstreams = input_items.size();

      const char **inv = (const char **) &input_items[0];
      char *out = (char *) output_items[0];

      for (int i = 0; i < noutput_items; i++){
	for (int j = 0; j < nstreams; j++){
	  memcpy(out, inv[j], itemsize);
	  inv[j] += itemsize;
	  out += itemsize;
	}
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
