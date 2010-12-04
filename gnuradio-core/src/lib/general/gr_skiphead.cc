/* -*- c++ -*- */
/*
 * Copyright 2005,2007,2010 Free Software Foundation, Inc.
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
#include <gr_skiphead.h>
#include <gr_io_signature.h>
#include <string.h>

gr_skiphead::gr_skiphead (size_t itemsize, uint64_t nitems_to_skip)
  : gr_block ("skiphead",
	      gr_make_io_signature(1, 1, itemsize),
	      gr_make_io_signature(1, 1, itemsize)),
    d_nitems_to_skip(nitems_to_skip), d_nitems(0)
{
}

gr_skiphead_sptr
gr_make_skiphead (size_t itemsize, uint64_t nitems_to_skip)
{
  return gnuradio::get_initial_sptr(new gr_skiphead (itemsize, nitems_to_skip));
}

int
gr_skiphead::general_work(int noutput_items,
			  gr_vector_int &ninput_items_ignored,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  const char *in = (const char *) input_items[0];
  char *out = (char *) output_items[0];

  int ninput_items = noutput_items;	// we've got at least this many input items
  int ii = 0;				// input index

  while (ii < ninput_items){

    uint64_t ni_total = ii + d_nitems;  	// total items processed so far
    if (ni_total < d_nitems_to_skip){		// need to skip some more

      int n_to_skip = (int) std::min(d_nitems_to_skip - ni_total,
				     (uint64_t)(ninput_items - ii));
      ii += n_to_skip;
    }

    else {		// nothing left to skip.  copy away

      int n_to_copy = ninput_items - ii;
      if (n_to_copy > 0){
	size_t itemsize = output_signature()->sizeof_stream_item(0);
	memcpy(out, in + (ii*itemsize), n_to_copy*itemsize);
      }

      d_nitems += ninput_items;
      consume_each(ninput_items);
      return n_to_copy;
    }
  }

  d_nitems += ninput_items;
  consume_each(ninput_items);
  return 0;
}
