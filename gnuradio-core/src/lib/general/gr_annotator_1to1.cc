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

#include <gr_annotator_1to1.h>
#include <gr_io_signature.h>
#include <string.h>
#include <iostream>
#include <iomanip>

gr_annotator_1to1_sptr
gr_make_annotator_1to1 (size_t sizeof_stream_item)
{
  return gnuradio::get_initial_sptr (new gr_annotator_1to1 (sizeof_stream_item));
}

gr_annotator_1to1::gr_annotator_1to1 (size_t sizeof_stream_item)
  : gr_sync_block ("annotator_1to1",
		   gr_make_io_signature (1, -1, sizeof_stream_item),
		   gr_make_io_signature (1, -1, sizeof_stream_item)),
    d_itemsize(sizeof_stream_item)
{
  set_tag_propagation_policy(TPP_ONE_TO_ONE);

  d_tag_counter = 0;
}

gr_annotator_1to1::~gr_annotator_1to1 ()
{
  std::cout << d_sout.str();
}

int
gr_annotator_1to1::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  const float **in = (const float **) &input_items[0];
  float **out = (float **) &output_items[0];

  std::stringstream str;
  str << name() << unique_id();

  uint64_t abs_N = nitems_read(0) + noutput_items;
  std::vector<pmt::pmt_t> all_tags = get_tags_in_range(0, (uint64_t)0, abs_N);
  std::vector<pmt::pmt_t>::iterator itr;

  d_sout << std::endl << "Found " << all_tags.size() << " tags." << std::endl;
  d_sout.setf(std::ios::left);
  d_sout << std::setw(25) << "Receiver" << std::setw(25) << "Sender"
	 << std::setw(10) << "nitem" << std::setw(20) << "key"
	 << std::setw(10) << "value" << std::endl;

  for(itr = all_tags.begin(); itr != all_tags.end(); itr++) {
    d_sout << std::setw(25) << str.str()
	   << std::setw(25) << pmt::pmt_tuple_ref(*itr, 1)
	   << std::setw(10) << pmt::pmt_tuple_ref(*itr, 0)
	   << std::setw(20) << pmt::pmt_tuple_ref(*itr, 2)
	   << std::setw(10) << pmt::pmt_tuple_ref(*itr, 3)
	   << std::endl;
  }
  

  // Storing the current noutput_items as the value to the "noutput_items" key
  pmt::pmt_t srcid = pmt::pmt_string_to_symbol(str.str());
  pmt::pmt_t key = pmt::pmt_string_to_symbol("seq");
  pmt::pmt_t value;

  // Work does nothing to the data stream; just copy all inputs to outputs
  int noutputs = output_items.size();
  int ninputs  = input_items.size();
  for (int i = 0; i < noutputs; i++) {
    value = pmt::pmt_from_uint64(d_tag_counter++);
    memcpy(out[i], in[i], noutput_items * d_itemsize);
    add_item_tag(i, abs_N, key, value, srcid);
  }

  return noutput_items;
}
