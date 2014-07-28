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

#include "vector_to_streams_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    vector_to_streams::sptr vector_to_streams::make(size_t itemsize, size_t nstreams)
    {
      return gnuradio::get_initial_sptr(new vector_to_streams_impl(itemsize, nstreams));
    }

    vector_to_streams_impl::vector_to_streams_impl(size_t itemsize, size_t nstreams)
      : sync_block ("vector_to_streams",
		       io_signature::make (1, 1, nstreams * itemsize),
		       io_signature::make (nstreams, nstreams, itemsize))

    {
    }

    int
    vector_to_streams_impl::work(int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
    {
      size_t itemsize = output_signature()->sizeof_stream_item(0);
      int nstreams = output_items.size();

      const char *in = (const char *) input_items[0];
      char **outv = (char **) &output_items[0];

      for (int i = 0; i < noutput_items; i++){
	for (int j = 0; j < nstreams; j++){
	  memcpy(outv[j], in, itemsize);
	  outv[j] += itemsize;
	  in += itemsize;
	}
      }

      return noutput_items;
    }
  } /* namespace blocks */
} /* namespace gr */
