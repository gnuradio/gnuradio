/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010 Free Software Foundation, Inc.
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

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gr_vector_source_i.h>
#include <algorithm>
#include <gr_io_signature.h>
#include <stdexcept>


gr_vector_source_i::gr_vector_source_i (const std::vector<int> &data, bool repeat, int vlen)
  : gr_sync_block ("vector_source_i",
	       gr_make_io_signature (0, 0, 0),
	       gr_make_io_signature (1, 1, sizeof (int) * vlen)),
    d_data (data),
    d_repeat (repeat),
    d_offset (0),
    d_vlen (vlen)
{
  if ((data.size() % vlen) != 0)
    throw std::invalid_argument("data length must be a multiple of vlen");
}

int
gr_vector_source_i::work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  int *optr = (int *) output_items[0];

  if (d_repeat){
    unsigned int size = d_data.size ();
    unsigned int offset = d_offset;
    
    if (size == 0)
      return -1;
    
    for (int i = 0; i < noutput_items*d_vlen; i++){
      optr[i] = d_data[offset++];
      if (offset >= size)
	offset = 0;
    }
    d_offset = offset;
    return noutput_items;
  }

  else {
    if (d_offset >= d_data.size ())
      return -1;			// Done!

    unsigned n = std::min ((unsigned) d_data.size () - d_offset,
			   (unsigned) noutput_items*d_vlen);
    for (unsigned i = 0; i < n; i++)
      optr[i] = d_data[d_offset + i];

    d_offset += n;
    return n/d_vlen;
  }
}

gr_vector_source_i_sptr
gr_make_vector_source_i (const std::vector<int> &data, bool repeat, int vlen)
{
  return gnuradio::get_initial_sptr(new gr_vector_source_i (data, repeat, vlen));
}

