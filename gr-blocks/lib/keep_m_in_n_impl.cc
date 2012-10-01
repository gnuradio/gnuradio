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

#include "keep_m_in_n_impl.h"
#include <gr_io_signature.h>

namespace gr {
  namespace blocks {

    keep_m_in_n::sptr keep_m_in_n::make(size_t itemsize, int m, int n, int offset)
    {
      return gnuradio::get_initial_sptr(new keep_m_in_n_impl(itemsize, m, n, offset));
    }

    keep_m_in_n_impl::keep_m_in_n_impl(size_t itemsize, int m, int n, int offset)
      : gr_block("keep_m_in_n",
		 gr_make_io_signature (1, 1, itemsize),
		 gr_make_io_signature (1, 1, itemsize)),
	d_itemsize(itemsize),
	d_m(m),
	d_n(n),
	d_offset(offset)
    {
      // sanity checking
      assert(d_m > 0);
      assert(d_n > 0);
      assert(d_m <= d_n);
      assert(d_offset <= (d_n-d_m));
      
      set_output_multiple(m);
    }

    void
    keep_m_in_n_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = d_n*(noutput_items/d_m);
    }

    int
    keep_m_in_n_impl::general_work(int noutput_items,
				   gr_vector_int &ninput_items,
				   gr_vector_const_void_star &input_items,
				   gr_vector_void_star &output_items)
    {
      uint8_t* out = (uint8_t*)output_items[0];
      const uint8_t* in = (const uint8_t*)input_items[0];
      
      // iterate over data blocks of size {n, input : m, output}
      int blks = std::min(noutput_items/d_m, ninput_items[0]/d_n);

      for(int i=0; i<blks; i++) {
	// set up copy pointers
	const uint8_t* iptr = &in[(i*d_n + d_offset)*d_itemsize];
	uint8_t* optr = &out[i*d_m*d_itemsize];
	// perform copy
	memcpy( optr, iptr, d_m*d_itemsize );
      }
      
      consume_each(d_n);
      return d_m;
    }
    
  } /* namespace blocks */
} /* namespace gr */
