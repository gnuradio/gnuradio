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

#include "streams_to_stream_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    streams_to_stream::sptr streams_to_stream::make(size_t itemsize, size_t nstreams)
    {
      return gnuradio::get_initial_sptr(new streams_to_stream_impl(itemsize, nstreams));
    }

    streams_to_stream_impl::streams_to_stream_impl(size_t itemsize, size_t nstreams)
      : sync_interpolator ("streams_to_stream",
			      io_signature::make (nstreams, nstreams, itemsize),
			      io_signature::make (1, 1, itemsize),
			      nstreams)
    {
    }

    int
    streams_to_stream_impl::work(int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
    {
      size_t itemsize = output_signature()->sizeof_stream_item (0);

      const char **inv = (const char **) &input_items[0];
      char *out = (char *) output_items[0];
      int nstreams = input_items.size();

      assert (noutput_items % nstreams == 0);
      int ni = noutput_items / nstreams;

      for (int i = 0; i < ni; i++){
	for (int j = 0; j < nstreams; j++){
	  memcpy(out, inv[j], itemsize);
	  out += itemsize;
	  inv[j] += itemsize;
	}
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
