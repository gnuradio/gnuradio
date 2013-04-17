/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2013 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include "head_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>

namespace gr {
  namespace blocks {

    head::sptr
    head::make(size_t sizeof_stream_item, uint64_t nitems)
    {
      return gnuradio::get_initial_sptr
        (new head_impl(sizeof_stream_item, nitems));
    }

    head_impl::head_impl(size_t sizeof_stream_item, uint64_t nitems)
      : sync_block("head",
                   io_signature::make(1, 1, sizeof_stream_item),
                   io_signature::make(1, 1, sizeof_stream_item)),
        d_nitems(nitems), d_ncopied_items(0)
    {
    }

    head_impl::~head_impl()
    {
    }

    int
    head_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
    {
      if(d_ncopied_items >= d_nitems)
        return -1;			// Done!

      unsigned n = std::min(d_nitems - d_ncopied_items,
                            (uint64_t)noutput_items);

      if(n == 0)
        return 0;

      memcpy(output_items[0], input_items[0],
             n * input_signature()->sizeof_stream_item (0));
      d_ncopied_items += n;

      return n;
    }

  } /* namespace blocks */
} /* namespace gr */
