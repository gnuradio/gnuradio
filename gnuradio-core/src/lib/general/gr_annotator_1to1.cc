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
gr_make_annotator_1to1 (size_t sizeof_stream_item, float rel_rate)
{
  return gnuradio::get_initial_sptr (new gr_annotator_1to1 (sizeof_stream_item, rel_rate));
}

gr_annotator_1to1::gr_annotator_1to1 (size_t sizeof_stream_item, float rel_rate)
  : gr_block ("annotator_1to1",
	      gr_make_io_signature (1, -1, sizeof_stream_item),
	      gr_make_io_signature (1, -1, sizeof_stream_item)),
    d_itemsize(sizeof_stream_item), d_rel_rate(rel_rate)
{
  set_tag_propagation_policy(TPP_ONE_TO_ONE);

  d_tag_counter = 0;
  set_relative_rate(d_rel_rate);
}

gr_annotator_1to1::~gr_annotator_1to1 ()
{
}

int
gr_annotator_1to1::general_work (int noutput_items,
				 gr_vector_int &ninput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
{
  const float *in = (const float*)input_items[0];
  float *out = (float*)output_items[0];

  std::stringstream str;
  str << name() << unique_id();

  uint64_t abs_N = nitems_read(0);
  std::vector<pmt::pmt_t> all_tags = get_tags_in_range(0, abs_N, abs_N + noutput_items);

  std::vector<pmt::pmt_t>::iterator itr;
  for(itr = all_tags.begin(); itr != all_tags.end(); itr++) {
    d_stored_tags.push_back(*itr);
  }

  // Storing the current noutput_items as the value to the "noutput_items" key
  pmt::pmt_t srcid = pmt::pmt_string_to_symbol(str.str());
  pmt::pmt_t key = pmt::pmt_string_to_symbol("seq");

  // Work does nothing to the data stream; just copy all inputs to outputs
  // Adds a new tag when the number of items read is a multiple of N
  uint64_t N = 10000;
  int noutputs = output_items.size();
  for(int j = 0; j < noutput_items; j++) {
    abs_N++;
    
    for(int i = 0; i < noutputs; i++) {
      if(abs_N % N == 0) {
	pmt::pmt_t value = pmt::pmt_from_uint64(d_tag_counter++);
	add_item_tag(i, abs_N, key, value, srcid);
      }

      in  = (const float*)input_items[i];
      out = (float*)output_items[i];
      out[j] = in[j];
    }
  }

  consume_each(noutput_items);
  return noutput_items;
}
