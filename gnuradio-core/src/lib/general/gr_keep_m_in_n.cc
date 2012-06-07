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

#include <gr_keep_m_in_n.h>
#include <gr_io_signature.h>
#include <string.h>
#include <stdio.h>

gr_keep_m_in_n_sptr
gr_make_keep_m_in_n(size_t item_size, int m, int n, int offset)
{
  return gnuradio::get_initial_sptr(new gr_keep_m_in_n(item_size, m, n, offset));
}


/*
*
* offset = 0, starts with 0th item
* offset = 1, starts with 1st item, etc...
*
* we take m items out of each n
*/
gr_keep_m_in_n::gr_keep_m_in_n(size_t item_size, int m, int n, int offset)
  : gr_sync_block ("keep_m_in_n",
	      gr_make_io_signature(1, 1, n*item_size),
	      gr_make_io_signature(1, 1, m*item_size)),
    d_n(n),
    d_m(m),
    d_offset(offset)
{
    // sanity checking
    assert(d_m > 0);
    assert(d_n > 0);
    assert(d_m <= d_n);
    assert(d_offset <= (d_n-d_m));
}


void gr_keep_m_in_n::set_offset(int offset)
{
    d_offset = offset;
}


int
gr_keep_m_in_n::work(int noutput_items,
		     gr_vector_const_void_star &input_items,
		     gr_vector_void_star &output_items)
{
  uint8_t* out = (uint8_t*)output_items[0];
  const uint8_t* in = (const uint8_t*)input_items[0];
  
  int in_item( input_signature()->sizeof_stream_item(0) );
  int out_item( output_signature()->sizeof_stream_item(0) );
  int single_size = in_item/d_n;

  // iterate over data blocks of size {n, input : m, output}
  for(int i = 0; i < noutput_items; i++) {
    memcpy(&out[out_item*i], &in[in_item*i + single_size*d_offset], out_item);
  }
 
  return noutput_items;
}
