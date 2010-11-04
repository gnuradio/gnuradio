/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <gr_random_annotator.h>
#include <gr_io_signature.h>
#include <string.h>
#include <iostream>

gr_random_annotator_sptr
gr_make_random_annotator (size_t sizeof_stream_item)
{
  return gnuradio::get_initial_sptr (new gr_random_annotator (sizeof_stream_item));
}

gr_random_annotator::gr_random_annotator (size_t sizeof_stream_item)
  : gr_sync_block ("random_annotator",
		   gr_make_io_signature (1, -1, sizeof_stream_item),
		   gr_make_io_signature (1, -1, sizeof_stream_item)),
    d_itemsize(sizeof_stream_item)
{
}

gr_random_annotator::~gr_random_annotator ()
{
}

int
gr_random_annotator::work (int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
{
  const float **in = (const float **) &input_items[0];
  float **out = (float **) &output_items[0];

  gr_uint64 abs_N = nitems_written(0);
  std::deque<pmt::pmt_t> all_tags = get_tags_in_range(0, (gr_uint64)0, abs_N);
  std::deque<pmt::pmt_t>::iterator itr;
  std::cout << std::endl << "Found " << all_tags.size() << " tags." << std::endl;
  for(itr = all_tags.begin(); itr != all_tags.end(); itr++) {
    gr_uint64 nitem = pmt::pmt_to_uint64(pmt::pmt_tuple_ref(*itr, 0));
    std::string srcid = pmt::pmt_symbol_to_string(pmt::pmt_tuple_ref(*itr, 1));
    std::string key   = pmt::pmt_symbol_to_string(pmt::pmt_tuple_ref(*itr, 2));
    gr_uint64 value = pmt::pmt_to_uint64(pmt::pmt_tuple_ref(*itr, 3));

    std::cout << "Tag at " << nitem << " from " << srcid
	      << " with key = \"" << key << "\" had value = " << value << std::endl;
  }
  
  // Work does nothing to the data stream; just copy all inputs to outputs
  int ninputs = input_items.size();
  for (int i = 0; i < ninputs; i++){
    memcpy(out[i], in[i], noutput_items * d_itemsize);
  }

  // Storing the current noutput_items as the value to the "noutput_items" key
  std::stringstream str;
  str << name() << unique_id();
  pmt::pmt_t cur_N = pmt::pmt_from_uint64(noutput_items);
  pmt::pmt_t srcid = pmt::pmt_string_to_symbol(str.str());
  pmt::pmt_t key = pmt::pmt_string_to_symbol("noutput_items");
  add_item_tag(0, abs_N, key, cur_N, srcid);

  return noutput_items;
}
